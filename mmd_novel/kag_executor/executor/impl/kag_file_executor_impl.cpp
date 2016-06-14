#include "kag_file_executor_impl.h"
#define GET(name) get(L#name, name)
namespace kag
{
  FileExecutor::Pimpl::Pimpl(const Executor& exe) :executor_(exe)
  {
    /* メッセージ関連 */

    //tag_func_[SnapShotSpan(L"cancelautomode")] = &FileExecutor::CancelAutoModeTag;
    //tag_func_[SnapShotSpan(L"cancelskip")] = &FileExecutor::CancelSkipTag;
    auto bind = [this](auto func) { return [=](CommandToken& token) { return (this->*func)(token); }; };
    tag_func_[SnapShotSpan(L"ch")] = bind(&Pimpl::CHTag);
    tag_func_[SnapShotSpan(L"cm")] = bind(&Pimpl::CMTag);
    tag_func_[SnapShotSpan(L"ct")] = bind(&Pimpl::CTTag);
    tag_func_[SnapShotSpan(L"current")] = bind(&Pimpl::CurrentTag);
    tag_func_[SnapShotSpan(L"deffont")] = bind(&Pimpl::DefFontTag);
    tag_func_[SnapShotSpan(L"defstyle")] = bind(&Pimpl::DefStyleTag);
    tag_func_[SnapShotSpan(L"delay")] = bind(&Pimpl::DelayTag);
    tag_func_[SnapShotSpan(L"endindent")] = bind(&Pimpl::EndIndentTag);
    tag_func_[SnapShotSpan(L"endnowait")] = bind(&Pimpl::EndNoWaitTag);
    tag_func_[SnapShotSpan(L"er")] = bind(&Pimpl::ERTag);
    tag_func_[SnapShotSpan(L"font")] = bind(&Pimpl::FontTTag);

    //tag_func_[SnapShotSpan(L"glyph")] = &Pimpl::GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &Pimpl::GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &Pimpl::HCH;
    tag_func_[SnapShotSpan(L"indent")] = bind(&Pimpl::IndentTag);
    tag_func_[SnapShotSpan(L"l")] = bind(&Pimpl::LTag);
    tag_func_[SnapShotSpan(L"locate")] = bind(&Pimpl::LocateTag);

    //tag_func_[SnapShotSpan(L"locklink")] = &Pimpl::LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = bind(&Pimpl::NoWaitTag);
    tag_func_[SnapShotSpan(L"p")] = bind(&Pimpl::PTag);
    tag_func_[SnapShotSpan(L"position")] = bind(&Pimpl::PositionTTag);
    tag_func_[SnapShotSpan(L"r")] = bind(&Pimpl::RTag);
    tag_func_[SnapShotSpan(L"resetfont")] = bind(&Pimpl::ResetFontTag);
    tag_func_[SnapShotSpan(L"resetstyle")] = bind(&Pimpl::ResetStyleTag);

    //tag_func_[SnapShotSpan(L"ruby")] = &Pimpl::RubyTag;
    tag_func_[SnapShotSpan(L"style")] = bind(&Pimpl::StyleTag);

    //tag_func_[SnapShotSpan(L"unlocklink")] = &Pimpl::UnlockLinkTag;

    /* 画像関連 */
    tag_func_[SnapShotSpan(L"image")] = bind(&Pimpl::ImageTag);

    /* レイヤ関連 */
    tag_func_[L"move"] = bind(&Pimpl::MoveTag);
    tag_func_[L"trans"] = bind(&Pimpl::TransTag);

    /* MMD関連 */
    tag_func_[L"camera"] = bind(&Pimpl::CameraTag);

  }

  void FileExecutor::Pimpl::Load(const FilePath & path)
  {
    parser_ = Parser(path);
  }

  void FileExecutor::Pimpl::Update()
  {
    if ( !executor_.Update() ) return;

    while ( executor_.CommandUpdate() )
    {
      switch ( parser_.nextType() )
      {
      case  kag::Parser::Type::Text:
        executor_.CommandText(parser_.readText());
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
          throw std::runtime_error(token.name().ToNarrow());
        }
        break;
      }

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  int FileExecutor::Pimpl::NowFileLine() const
  {
    return parser_.NowLine();
  }

  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
  タグの実装
  - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void FileExecutor::Pimpl::LTag(CommandToken &)
  {
    executor_.CommandL();
  }

  void FileExecutor::Pimpl::LocateTag(CommandToken & token)
  {
    Value<int> x, y;
    if ( token.GET(x).GET(y).HasError() ) return;
    executor_.CommandLocate(x, y);
  }

  void FileExecutor::Pimpl::RTag(CommandToken &)
  {
    executor_.CommandR();
  }

  void FileExecutor::Pimpl::ResetFontTag(CommandToken &)
  {
    executor_.CommandResetFont();
  }

  void FileExecutor::Pimpl::ResetStyleTag(CommandToken &)
  {
    executor_.CommandResetStyle();
  }

  void FileExecutor::Pimpl::StyleTag(CommandToken & token)
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
    StyleVal tag(token, executor_);
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

  /* レイヤ関連 */

  void FileExecutor::Pimpl::ImageTag(CommandToken & token)
  {
    struct ImageVal
    {
      Must<std::pair<converter::LayerType, int>> layer;
      LayerPage page = LayerPage::Fore;
      Must<SnapShotSpan> storage;
      Optional<int> left, top;
      Optional<int> opacity;
      Optional<int> index;
      Optional<bool> visible;

      Executor exe_;
      ImageVal(CommandToken& token, const Executor& exe) :exe_(exe)
      {
        token.GET(layer).GET(page).GET(storage).GET(left).GET(top)
          .GET(opacity).GET(index).GET(visible);
      }
      void attach() const
      {
        auto ptr = exe_.imageManager()->GetLayer(layer->second)[page];
        ptr->SetTex(Texture(storage->ToStr()));
        if ( left ) ptr->SetPositionLeft(*left);
        if ( top ) ptr->SetPositionLeft(*top);
        if ( index ) ptr->SetZIndex(*index);
        if ( visible ) ptr->IsVisible(*visible);
      }
    };
    ImageVal tag(token, executor_);
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)](){
      tag.attach();
    });
  }

  void FileExecutor::Pimpl::MoveTag(CommandToken & token)
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
    MoveVal tag(token, executor_);
    if ( token.HasError() ) return;
    executor_.Command(tag.func);
  }

  void FileExecutor::Pimpl::TransTag(CommandToken & token)
  {
    struct TransVal
    {
      std::pair<converter::LayerType, int> layer = { converter::LayerType::Background,0 };
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
    TransVal tag(token, executor_);
    if ( token.HasError() )return;
    executor_.Command(tag.func);
  }

  /* MMD関連 */


  void FileExecutor::Pimpl::CameraTag(CommandToken & token)
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
    executor_.Command([tag = CameraVal(token)](){ tag.attach(); });
  }

  void FileExecutor::Pimpl::NoWaitTag(CommandToken &)
  {
    executor_.CommandNoWait();
  }

  void FileExecutor::Pimpl::PTag(CommandToken &)
  {
    executor_.CommandP();
  }

  void FileExecutor::Pimpl::DelayTag(CommandToken & token)
  {
    Must<SnapShotSpan> speed;
    if ( token.GET(speed).HasError() ) return;
    if ( *speed == L"user" )
    {
      executor_.CommandDelay(30);
    }
    else if ( *speed == L"nowait" )
    {
      executor_.CommandNoWait();
    }
    else
    {
      int val;
      if ( converter::TryConvert(*speed, val) )
      {
        executor_.CommandDelay(val);
      }
      else
      {
        token.AddIllegalException(L"speed", *speed);
      }
    }

  }

  void FileExecutor::Pimpl::EndIndentTag(CommandToken &)
  {
    executor_.CommandEndIndent();
  }

  void FileExecutor::Pimpl::EndNoWaitTag(CommandToken &)
  {
    executor_.CommandEndNoWait();
  }

  void FileExecutor::Pimpl::ERTag(CommandToken &)
  {
    executor_.CommandER();
  }

  void FileExecutor::Pimpl::CMTag(CommandToken &)
  {
    executor_.CommandCM();
  }

  void FileExecutor::Pimpl::CTTag(CommandToken &)
  {
    executor_.CommandCT();
  }

  void FileExecutor::Pimpl::CurrentTag(CommandToken & token)
  {
    std::pair<converter::LayerType, int> layer = { converter::LayerType::Message,Define::default };
    LayerPage page = LayerPage::Fore;

    using namespace converter;
    if ( token.GET(layer).GET(page).HasError() )
      return;
    executor_.CommandCurrent(layer.second, page);
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

  void FileExecutor::Pimpl::DefFontTag(CommandToken & token)
  {
    DefFont tag(token, executor_);
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::DefStyleTag(CommandToken & token)
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
    executor_.Command([tag = DefStyleVal(token, executor_)](){ tag.attach(); });
  }

  void FileExecutor::Pimpl::FontTTag(CommandToken & token)
  {
    executor_.Command([tag = FontVal(token, executor_)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::IndentTag(CommandToken &)
  {
    executor_.CommandIndent();
  }

  void FileExecutor::Pimpl::PositionTTag(CommandToken & token)
  {
    struct PositionVal
    {
      PositionVal(CommandToken & token, const Executor& exe) :exe_(exe)
      {
        token.GET(layer).GET(page).GET(left).GET(top).GET(width).GET(height)
          .GET(marginl).GET(margint).GET(marginr).GET(marginb)
          .GET(color).GET(frame).GET(opacity).GET(visible);
        if ( layer && layer->first != converter::LayerType::Message )
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
    PositionVal tag(token, executor_);
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::CHTag(CommandToken & token)
  {
    Must<SnapShotSpan> text;
    if ( token.GET(text).HasError() ) return;
    executor_.CommandTextNoDelay(*text);
  }

}
