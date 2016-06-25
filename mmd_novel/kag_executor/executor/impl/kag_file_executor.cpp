#include <MmdNovel/kag_file_executor.h>
#include <MmdNovel/message/message_manager.h>
#define GET(name) get(L#name, name)
namespace kag
{
  namespace file
  {
    FileExecutor::FileExecutor(const FilePath& path) :FileExecutor()
    {
      Load(path);
    }

    FileExecutor::FileExecutor()
    {
      /* メッセージ関連 */

      //tag_func_[SnapShotSpan(L"cancelautomode")] = &FileExecutor::CancelAutoModeTag;
      //tag_func_[SnapShotSpan(L"cancelskip")] = &FileExecutor::CancelSkipTag;


      //tag_func_[SnapShotSpan(L"unlocklink")] = &UnlockLinkTag;
      auto bind = [this](auto func) { return [this, func](CommandToken& token) { (this->*func)(token); }; };
      /* レイヤ関連 */
      tag_func_[L"move"] = bind(&FileExecutor::MoveTag);
      tag_func_[L"trans"] = bind(&FileExecutor::TransTag);

      /* MMD関連 */
      tag_func_[L"camera"] = bind(&FileExecutor::CameraTag);

    }

    void FileExecutor::Load(const FilePath & path)
    {
      parser_ = Parser(path);
    }

    void FileExecutor::Update()
    {
      if ( !Executor::Update() ) return;

      while ( Executor::CommandUpdate() )
      {
        switch ( parser_.nextType() )
        {
        case Parser::Type::Text:
          GetManager<MessageManager>(L"message")->Append(parser_.readText());
          break;

        case Parser::Type::Command:
        {
          auto token = parser_.readCommand();
          auto func = tag_func_[token.name()];
          if ( func )
          {
            func(token);
            auto& errors = token.Errors();
            if ( errors.size() )
            {
              Println(L'[', token.name(), L"]タグでエラーが発生");
              LOG_ERROR(L'[', token.name(), L"]タグでエラーが発生");
            }
            for ( auto& i : errors )
            {
              if ( i.type == Error::Type::IllegalArgument )
              {
                String error = Format(LR"(  {}行目: "{}"属性 "{}" は不正な値です)"_fmt,
                  i.arg_val.Line(), i.arg_name.ToStr(), i.arg_val.ToStr());
                Println(error);
                LOG_ERROR(error);
              }
              else
              {
                String error = Format(LR"(  {}行目 :"{}"属性は必須です)"_fmt,
                  token.name().Line(), i.arg_name.ToStr());
                Println(error);
                LOG_ERROR(error);
              }
            }
          }
          else
          {
            Println(L'[', token.name(), L"]タグは存在しません");
            LOG_ERROR(L'[', token.name(), L"]タグは存在しません");
          }
          break;
        }

        case Parser::Type::EndOfStream:
          return;
        }
      }
    }

    int FileExecutor::NowFileLine() const
    {
      return parser_.NowLine();
    }

    void FileExecutor::AddManager(const SnapShotSpan & name, const std::shared_ptr<detail::IFileManager>& manager)
    {
      Executor::AddManager(name, manager);
      manager->AddTag(tag_func_);
    }

    /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
    タグの実装
    - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */

    /* レイヤ関連 */

    void FileExecutor::MoveTag(CommandToken & token)
    {
      struct MoveVal
      {
        Must<std::pair<LayerType, int>> layer;
        LayerPage page = LayerPage::Fore;
        Must<int> time;
        Must<SnapShotSpan> path;

        std::function<void()> func;
        MoveVal(CommandToken& token, const Executor& exe)
        {
          if ( token.GET(layer).GET(page).GET(time).GET(path).HasError() ) return;

          auto buf = path->Str();
          int n = static_cast<int>(std::count(buf, buf + path->Length(), L'('));
          Vec2 s = { 0,0 }, e;
          int n_pos = 0;
          MoveEffectData::Array data;
          for ( int i = 0; i < n; i++ )
          {
            int opacity;
            if ( swscanf(buf + n_pos, L"(%lf,%lf,%d)%n", &e.x, &e.y, &opacity, &n_pos) != 3 )
            {
              throw std::runtime_error(path->ToNarrow());
            }
            data.push_back(MoveEffectData(s, e, Easing::Linear, EasingType::Type::In, *time));
            std::swap(s, e);
          }
          func = [data, layer = *layer, page = page, exe]() mutable{
            auto ptr = exe.GetLayer(layer)[page];
            data[0].start = ptr->position().pos;
            ptr->MoveEffect(data);
          };
        }
      };
      MoveVal tag(token, *this);
      if ( token.HasError() ) return;
      Executor::Command(tag.func);
    }

    void FileExecutor::TransTag(CommandToken & token)
    {
      struct TransVal
      {
        std::pair<LayerType, int> layer = { L"base", 0 };
        Must<int> time;
        SnapShotSpan method = L"universal";
        Must<SnapShotSpan> rule;
        Must<int> vague;

        std::function<void()> func;

        TransVal(CommandToken& token, const Executor& exe)
        {
          if ( token.GET(layer).GET(time).GET(method).HasError() ) return;
          if ( method == L"universal" )
          {
            if ( token.GET(rule).GET(vague).HasError() ) return;
            TransUniversalData data;
            data.rule_tex = Texture(rule->ToStr());
            data.vague = *vague;
            data.time_millisec = *time;
            func = [data, exe, layer = layer]() mutable { exe.GetLayer(layer).Trans(data); };
          }
          else if ( method == L"crossfade" )
          {
            func = [time_millisec = *time, exe, layer = layer]() mutable { exe.GetLayer(layer).Trans(time_millisec); };
          }
          else
          {
            token.AddIllegalException(L"method", method);
          }
        }
      };
      TransVal tag(token, *this);
      if ( token.HasError() )return;
      Executor::Command(tag.func);
    }

    /* MMD関連 */


    void FileExecutor::CameraTag(CommandToken & token)
    {
      struct CameraVal
      {
        Optional<Vec3> pos_, lookat_;
        CameraVal(CommandToken& token)
        {
          Optional<SnapShotSpan> pos, lookat;
          token.GET(pos).GET(lookat);
          Vec3 tmp;
          if ( pos && swscanf(pos->Str(), L"(%lf,%lf,%lf)", &tmp.x, &tmp.y, &tmp.z) == 3 )
            pos_ = tmp;
          if ( lookat && swscanf(lookat->Str(), L"(%lf,%lf,%lf)", &tmp.x, &tmp.y, &tmp.z) == 3 )
          {
            lookat_ = tmp;
          }
        }
        void attach() const
        {
          Camera camera;
          if ( pos_ ) camera.pos = *pos_;
          if ( lookat_ ) camera.lookat = *lookat_;
          Graphics3D::SetCamera(camera);
        }
      };
      Executor::Command([tag = CameraVal(token)](){ tag.attach(); });
    }

    int FileExecutor::NowLine() const
    {
      return parser_.NowLine();
    }



  }
}
