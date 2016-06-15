#include <MmdNovel/kag_file_executor.h>
#define GET(name) get(L#name, name)
namespace kag
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
    auto bind = [this](auto func) { return [=](CommandToken& token) { return (this->*func)(token); }; };
    tag_func_[SnapShotSpan(L"ch")] = bind(&FileExecutor::CHTag);
    tag_func_[SnapShotSpan(L"cm")] = bind(&FileExecutor::CMTag);
    tag_func_[SnapShotSpan(L"ct")] = bind(&FileExecutor::CTTag);
    tag_func_[SnapShotSpan(L"current")] = bind(&FileExecutor::CurrentTag);
    tag_func_[SnapShotSpan(L"deffont")] = bind(&FileExecutor::DefFontTag);
    tag_func_[SnapShotSpan(L"defstyle")] = bind(&FileExecutor::DefStyleTag);
    tag_func_[SnapShotSpan(L"delay")] = bind(&FileExecutor::DelayTag);
    tag_func_[SnapShotSpan(L"endindent")] = bind(&FileExecutor::EndIndentTag);
    tag_func_[SnapShotSpan(L"endnowait")] = bind(&FileExecutor::EndNoWaitTag);
    tag_func_[SnapShotSpan(L"er")] = bind(&FileExecutor::ERTag);
    tag_func_[SnapShotSpan(L"font")] = bind(&FileExecutor::FontTTag);

    //tag_func_[SnapShotSpan(L"glyph")] = &GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &HCH;
    tag_func_[SnapShotSpan(L"indent")] = bind(&FileExecutor::IndentTag);
    tag_func_[SnapShotSpan(L"l")] = bind(&FileExecutor::LTag);
    tag_func_[SnapShotSpan(L"locate")] = bind(&FileExecutor::LocateTag);

    //tag_func_[SnapShotSpan(L"locklink")] = &LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = bind(&FileExecutor::NoWaitTag);
    tag_func_[SnapShotSpan(L"p")] = bind(&FileExecutor::PTag);
    tag_func_[SnapShotSpan(L"position")] = bind(&FileExecutor::PositionTTag);
    tag_func_[SnapShotSpan(L"r")] = bind(&FileExecutor::RTag);
    tag_func_[SnapShotSpan(L"resetfont")] = bind(&FileExecutor::ResetFontTag);
    tag_func_[SnapShotSpan(L"resetstyle")] = bind(&FileExecutor::ResetStyleTag);

    //tag_func_[SnapShotSpan(L"ruby")] = &RubyTag;
    tag_func_[SnapShotSpan(L"style")] = bind(&FileExecutor::StyleTag);

    //tag_func_[SnapShotSpan(L"unlocklink")] = &UnlockLinkTag;

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
      case  kag::Parser::Type::Text:
        Executor::CommandText(parser_.readText());
        break;

      case  kag::Parser::Type::Command:
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

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  int FileExecutor::NowFileLine() const
  {
    return parser_.NowLine();
  }

  void FileExecutor::AddManager(const SnapShotSpan & name, const std::shared_ptr<IFileManager>& manager)
  {
    Executor::AddManager(name, manager);
    manager->AddTag(tag_func_);
  }

  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
  タグの実装
  - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void FileExecutor::LTag(CommandToken &)
  {
    Executor::CommandL();
  }

  void FileExecutor::LocateTag(CommandToken & token)
  {
    Value<int> x, y;
    if ( token.GET(x).GET(y).HasError() ) return;
    Executor::CommandLocate(x, y);
  }

  void FileExecutor::RTag(CommandToken &)
  {
    Executor::CommandR();
  }

  void FileExecutor::ResetFontTag(CommandToken &)
  {
    Executor::CommandResetFont();
  }

  void FileExecutor::ResetStyleTag(CommandToken &)
  {
    Executor::CommandResetStyle();
  }

  void FileExecutor::StyleTag(CommandToken & token)
  {
    struct StyleVal
    {
      Optional<int> linesize_;
      Optional<int> linespacing;

      Executor exe_;
      StyleVal(CommandToken& token, const Executor& exe) : exe_(exe)
      {
        Optional<SnapShotSpan> linesize;
        if ( token.GET(linesize).GET(linespacing).HasError() ) return;
        if ( linesize )
        {
          if ( *linesize == L"default" )
          {
            linesize_ = message::Style::default_line_size;
          }
          else
          {
            linesize_ = converter::ToInt10(*linesize);
          }
        }
      }
      void attach() const
      {
        auto& layer = exe_.messageManager().Current();
        if ( linesize_ ) layer->SetLineSize(*linesize_);
      }
    };
    StyleVal tag(token, *this);
    if ( token.HasError() ) return;
    Executor::Command([tag = std::move(tag)]() { tag.attach(); });
  }

  /* レイヤ関連 */



  void FileExecutor::MoveTag(CommandToken & token)
  {
    struct MoveVal
    {
      Must<std::pair<converter::LayerType, int>> layer;
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
      std::pair<converter::LayerType, int> layer = { L"base", 0 };
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

  void FileExecutor::NoWaitTag(CommandToken &)
  {
    Executor::CommandNoWait();
  }

  void FileExecutor::PTag(CommandToken &)
  {
    Executor::CommandP();
  }

  void FileExecutor::DelayTag(CommandToken & token)
  {
    Must<SnapShotSpan> speed;
    if ( token.GET(speed).HasError() ) return;
    if ( *speed == L"user" )
    {
      Executor::CommandDelay(30);
    }
    else if ( *speed == L"nowait" )
    {
      Executor::CommandNoWait();
    }
    else
    {
      int val;
      if ( converter::TryConvert(*speed, val) )
      {
        Executor::CommandDelay(val);
      }
      else
      {
        token.AddIllegalException(L"speed", *speed);
      }
    }

  }

  void FileExecutor::EndIndentTag(CommandToken &)
  {
    Executor::CommandEndIndent();
  }

  void FileExecutor::EndNoWaitTag(CommandToken &)
  {
    Executor::CommandEndNoWait();
  }

  void FileExecutor::ERTag(CommandToken &)
  {
    Executor::CommandER();
  }

  void FileExecutor::CMTag(CommandToken &)
  {
    Executor::CommandCM();
  }

  void FileExecutor::CTTag(CommandToken &)
  {
    Executor::CommandCT();
  }

  void FileExecutor::CurrentTag(CommandToken & token)
  {
    std::pair<converter::LayerType, int> layer = { L"message", Define::default };
    LayerPage page = LayerPage::Fore;

    using namespace converter;
    if ( token.GET(layer).GET(page).HasError() )
      return;
    Executor::CommandCurrent(layer.second, page);
  }

  namespace
  {
    struct FontVal
    {
      Executor exe_;
      Optional<SnapShotSpan> face;
      Optional<int> size;
      Optional<bool> italic, bold, shadow;
      Optional<Color> color, shadowcolor;
      virtual void commit(const MessageLayer& layer, message::TextFont new_font) const
      {
        layer->SetFont(new_font);
      }
      void attach()const
      {
        auto manager = exe_.messageManager();
        auto now_font = manager.Current()->NowFont();
        FontProperty prop;
        prop.name = face ? face->ToStr() : now_font.font_.name();
        prop.size = size ? *size : now_font.font_.size();
        prop.style = CreateStyle();
        message::TextFont new_font = now_font;
        new_font.font_ = Font(prop);
        if ( color ) new_font.color_ = *color;
        if ( shadow ) new_font.is_shadow_ = *shadow;
        if ( shadowcolor ) new_font.shadow_color_ = *shadowcolor;
        commit(manager.Current(), new_font);
      }
      FontStyle CreateStyle() const
      {
        int style_cnt = (italic == true) + (bold == true) * 2;
        switch ( style_cnt )
        {
        case 1: return FontStyle::Italic;
        case 2: return FontStyle::Bold;
        case 3: return FontStyle::BoldItalic;
        default:return FontStyle::Regular;
        }
      }
      FontVal(CommandToken& token, Executor exe)
      {
        token.GET(face).GET(size).GET(italic).GET(bold).GET(shadow).GET(color).GET(shadowcolor);
        exe_ = exe;
      }
    };
    struct DefFont : FontVal
    {
      using FontVal::FontVal;
      void commit(const MessageLayer& layer, message::TextFont new_font) const override
      {
        layer->SetDefaultFont(new_font);
      }
    };
  }

  void FileExecutor::DefFontTag(CommandToken & token)
  {
    DefFont tag(token, *this);
    if ( token.HasError() ) return;
    Executor::Command([tag = std::move(tag)]() { tag.attach(); });
  }

  void FileExecutor::DefStyleTag(CommandToken & token)
  {
    struct DefStyleVal
    {
      DefStyleVal(CommandToken & token, const Executor& exe)
      {
        token.GET(linesize).GET(linespacing).GET(pitch);
        exe_ = exe;
      }
      void attach() const
      {
        message::DefaultStyle style;
        style.line_size_ = linesize;
        style.line_spacing_ = linespacing;
        style.pitch_ = pitch;
        exe_.messageManager().Current()->SetDefaultStyle(style);
      }
      int linesize;
      int linespacing;
      int pitch;
      Executor exe_;
    };
    Executor::Command([tag = DefStyleVal(token, *this)](){ tag.attach(); });
  }

  void FileExecutor::FontTTag(CommandToken & token)
  {
    Executor::Command([tag = FontVal(token, *this)]() { tag.attach(); });
  }

  void FileExecutor::IndentTag(CommandToken &)
  {
    Executor::CommandIndent();
  }

  void FileExecutor::PositionTTag(CommandToken & token)
  {
    struct PositionVal
    {
      PositionVal(CommandToken & token, const Executor& exe) :exe_(exe)
      {
        token.GET(layer).GET(page).GET(left).GET(top).GET(width).GET(height)
          .GET(marginl).GET(margint).GET(marginr).GET(marginb)
          .GET(color).GET(frame).GET(opacity).GET(visible);
        if ( layer && layer->first != L"message" )
          token.AddIllegalException(L"layer", L""); // TODO:パースした元の値を取得する方法を考える
      }
      Optional<std::pair<converter::LayerType, int>> layer;
      Optional<LayerPage> page; // 省略時 カレントページ
      Optional<int> left, top, width, height;
      Optional<int> marginl, margint, marginr, marginb;
      Optional<Color> color;
      Optional<SnapShotSpan> frame;
      Optional<int> opacity;
      Optional<bool> visible;
      Executor exe_;
      void attach()const
      {
        auto manager = exe_.messageManager();
        int start = layer ? layer->second : manager.CurrentLayerNum();
        int end = start + 1;
        if ( start == Define::default )
        {
          end = manager.size();
          start = 0;
        }
        const LayerPage p = page ? *page : manager.CurrentPage();
        for ( int i = start; i < end; i++ )
        {
          MessageLayer ptr = manager.GetLayer(i, p);
          if ( left ) ptr->SetPositionLeft(*left);
          if ( top ) ptr->SetPositionTop(*top);
          if ( width ) ptr->SetPositionWidth(*width);
          if ( height ) ptr->SetPositionHeight(*height);

          if ( marginl ) ptr->SetMarginLeft(*marginl);
          if ( margint ) ptr->SetMarginLeft(*margint);
          if ( marginr ) ptr->SetMarginLeft(*marginr);
          if ( marginb ) ptr->SetMarginLeft(*marginb);

          if ( color ) ptr->SetBackgroundRGB(color->r, color->g, color->b);
          if ( frame ) ptr->SetBackgroundTex(Texture(frame->ToStr()));
          if ( opacity ) ptr->SetBackgroundOpacity(*opacity);
          if ( visible ) ptr->IsVisible(*visible);
        }
      }
    };
    PositionVal tag(token, *this);
    if ( token.HasError() ) return;
    Executor::Command([tag = std::move(tag)]() { tag.attach(); });
  }

  int FileExecutor::NowLine() const
  {
    return parser_.NowLine();
  }

  void FileExecutor::CHTag(CommandToken & token)
  {
    Must<SnapShotSpan> text;
    if ( token.GET(text).HasError() ) return;
    Executor::CommandTextNoDelay(*text);
  }

}
