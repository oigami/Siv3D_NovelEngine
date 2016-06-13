#include "kag_file_executor_impl.h"
#define GET(name) get(L#name, name)
namespace kag {

  FileExecutor::Pimpl::Pimpl(const Executor& exe) :executor_(exe)
  {
    /* メッセージ関連 */

    //tag_func_[SnapShotSpan(L"cancelautomode")] = &FileExecutor::CancelAutoModeTag;
    //tag_func_[SnapShotSpan(L"cancelskip")] = &FileExecutor::CancelSkipTag;
    tag_func_[SnapShotSpan(L"ch")] = &Pimpl::CHTag;
    tag_func_[SnapShotSpan(L"cm")] = &Pimpl::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &Pimpl::CTTag;
    tag_func_[SnapShotSpan(L"current")] = &Pimpl::CurrentTag;
    tag_func_[SnapShotSpan(L"deffont")] = &Pimpl::DefFontTag;
    tag_func_[SnapShotSpan(L"defstyle")] = &Pimpl::DefStyleTag;
    tag_func_[SnapShotSpan(L"delay")] = &Pimpl::DelayTag;
    tag_func_[SnapShotSpan(L"endindent")] = &Pimpl::EndIndentTag;
    tag_func_[SnapShotSpan(L"endnowait")] = &Pimpl::EndNoWaitTag;
    tag_func_[SnapShotSpan(L"er")] = &Pimpl::ERTag;
    tag_func_[SnapShotSpan(L"font")] = &Pimpl::FontTTag;

    //tag_func_[SnapShotSpan(L"glyph")] = &Pimpl::GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &Pimpl::GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &Pimpl::HCH;
    tag_func_[SnapShotSpan(L"indent")] = &Pimpl::IndentTag;
    tag_func_[SnapShotSpan(L"l")] = &Pimpl::LTag;
    tag_func_[SnapShotSpan(L"locate")] = &Pimpl::LocateTag;

    //tag_func_[SnapShotSpan(L"locklink")] = &Pimpl::LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = &Pimpl::NoWaitTag;
    tag_func_[SnapShotSpan(L"p")] = &Pimpl::PTag;
    tag_func_[SnapShotSpan(L"position")] = &Pimpl::PositionTTag;
    tag_func_[SnapShotSpan(L"r")] = &Pimpl::RTag;
    tag_func_[SnapShotSpan(L"resetfont")] = &Pimpl::ResetFontTag;
    tag_func_[SnapShotSpan(L"resetstyle")] = &Pimpl::ResetStyleTag;

    //tag_func_[SnapShotSpan(L"ruby")] = &Pimpl::RubyTag;
    tag_func_[SnapShotSpan(L"style")] = &Pimpl::StyleTag;

    //tag_func_[SnapShotSpan(L"unlocklink")] = &Pimpl::UnlockLinkTag;

    /* 画像関連 */
    tag_func_[SnapShotSpan(L"image")] = &Pimpl::ImageTag;

    /* レイヤ関連 */
    tag_func_[L"move"] = &Pimpl::MoveTag;
    tag_func_[L"trans"] = &Pimpl::TransTag;

    /* MMD関連 */
    tag_func_[L"mmd"] = &Pimpl::MMDTag;
    tag_func_[L"camera"] = &Pimpl::CameraTag;

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
          (this->*func)(token);
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
  void FileExecutor::Pimpl::LTag(Parser::CommandToken &)
  {
    executor_.CommandL();
  }

  void FileExecutor::Pimpl::LocateTag(Parser::CommandToken & token)
  {
    Value<int> x, y;
    token.GET(x).GET(y);
    executor_.CommandLocate(x, y);
  }

  void FileExecutor::Pimpl::RTag(Parser::CommandToken &)
  {
    executor_.CommandR();
  }

  void FileExecutor::Pimpl::ResetFontTag(Parser::CommandToken &)
  {
    executor_.CommandResetFont();
  }

  void FileExecutor::Pimpl::ResetStyleTag(Parser::CommandToken &)
  {
    executor_.CommandResetStyle();
  }

  void FileExecutor::Pimpl::StyleTag(Parser::CommandToken & token)
  {
    struct StyleVal
    {
      Optional<int> linesize_;
      Optional<int> linespacing;

      Executor exe_;
      StyleVal(Parser::CommandToken& token, const Executor& exe) : exe_(exe)
      {
        Optional<SnapShotSpan> linesize;
        token.GET(linesize).GET(linespacing);
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
    executor_.Command([tag = StyleVal(token, executor_)]() { tag.attach(); });
  }

  /* レイヤ関連 */

  void FileExecutor::Pimpl::ImageTag(Parser::CommandToken & token)
  {
    struct ImageVal
    {
      Parser::Must<std::pair<converter::LayerType, int>> layer;
      LayerPage page = LayerPage::Fore;
      Parser::Must<SnapShotSpan> storage;
      Optional<int> left, top;
      Optional<int> opacity;
      Optional<int> index;
      Optional<bool> visible;

      Executor exe_;
      ImageVal(Parser::CommandToken& token, const Executor& exe) :exe_(exe)
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
    executor_.Command([tag = ImageVal(token, executor_)](){
      tag.attach();
    });
  }

  void FileExecutor::Pimpl::MoveTag(Parser::CommandToken & token)
  {
    struct MoveVal
    {
      Parser::Must<std::pair<converter::LayerType, int>> layer;
      LayerPage page = LayerPage::Fore;
      Parser::Must<int> time;
      Parser::Must<SnapShotSpan> path;

      std::function<void()> func;
      MoveVal(Parser::CommandToken& token, const Executor& exe)
      {
        token.GET(layer).GET(page).GET(time).GET(path);

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
    executor_.Command(MoveVal(token, executor_).func);
  }

  void FileExecutor::Pimpl::TransTag(Parser::CommandToken & token)
  {
    struct TransVal
    {
      std::pair<converter::LayerType, int> layer = { converter::LayerType::Background,0 };
      Parser::Must<int> time;
      SnapShotSpan method = L"universal";
      Parser::Must<SnapShotSpan> rule;
      Parser::Must<int> vague;

      std::function<void()> func;

      TransVal(Parser::CommandToken& token, const Executor& exe)
      {
        token.GET(layer).GET(time).GET(method);
        if ( method == L"universal" )
        {
          token.GET(rule).GET(vague);
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
          throw std::runtime_error(method.ToNarrow());
        }
      }
    };
    executor_.Command(TransVal(token, executor_).func);
  }

  /* MMD関連 */

  void FileExecutor::Pimpl::MMDTag(Parser::CommandToken & token)
  {
    struct MMDVal
    {
      MMDVal(Parser::CommandToken& token, const Executor& exe)
      {
        exe_ = exe;
        token.GET(storage).GET(vmd).GET(loop).GET(start_time).GET(visible).GET(page);
      }
      Optional<SnapShotSpan> storage, vmd;
      LayerPage page = LayerPage::Fore;
      bool loop = true;
      int start_time = 0;
      bool visible = true;
      Executor exe_;
      void attach() const
      {
        auto layer = exe_.mmdLayer()[page];
        layer->IsVisible(visible);
        if ( storage ) layer->SetModel(storage->ToStr());
        if ( vmd ) layer->SetVMD(vmd->ToStr());
        layer->IsLoop(loop);
        layer->SetTime(start_time);
      }
    };
    executor_.Command([tag = MMDVal(token, executor_)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::CameraTag(Parser::CommandToken & token)
  {
    struct CameraVal
    {
      Optional<Vec3> pos_, lookat_;
      CameraVal(Parser::CommandToken& token)
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

  void FileExecutor::Pimpl::NoWaitTag(Parser::CommandToken &)
  {
    executor_.CommandNoWait();
  }

  void FileExecutor::Pimpl::PTag(Parser::CommandToken &)
  {
    executor_.CommandP();
  }

  void FileExecutor::Pimpl::DelayTag(Parser::CommandToken & token)
  {
    Parser::Must<SnapShotSpan> speed;
    token.GET(speed);
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

  void FileExecutor::Pimpl::EndIndentTag(Parser::CommandToken &)
  {
    executor_.CommandEndIndent();
  }

  void FileExecutor::Pimpl::EndNoWaitTag(Parser::CommandToken &)
  {
    executor_.CommandEndNoWait();
  }

  void FileExecutor::Pimpl::ERTag(Parser::CommandToken &)
  {
    executor_.CommandER();
  }

  void FileExecutor::Pimpl::CMTag(Parser::CommandToken &)
  {
    executor_.CommandCM();
  }

  void FileExecutor::Pimpl::CTTag(Parser::CommandToken &)
  {
    executor_.CommandCT();
  }

  void FileExecutor::Pimpl::CurrentTag(Parser::CommandToken & token)
  {
    std::pair<converter::LayerType, int> layer = { converter::LayerType::Message,Define::default };
    LayerPage page = LayerPage::Fore;

    using namespace converter;
    token.GET(layer).GET(page);
    executor_.CommandCurrent(layer.second, page);
  }

  namespace {
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
      FontVal(Parser::CommandToken& token, Executor exe)
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

  void FileExecutor::Pimpl::DefFontTag(Parser::CommandToken & token)
  {
    executor_.Command([tag = DefFont(token, executor_)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::DefStyleTag(Parser::CommandToken & token)
  {
    struct DefStyleVal
    {
      DefStyleVal(Parser::CommandToken & token, const Executor& exe)
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

  void FileExecutor::Pimpl::FontTTag(Parser::CommandToken & token)
  {
    executor_.Command([tag = FontVal(token, executor_)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::IndentTag(Parser::CommandToken &)
  {
    executor_.CommandIndent();
  }

  void FileExecutor::Pimpl::PositionTTag(Parser::CommandToken & token)
  {
    struct PositionVal
    {
      PositionVal(Parser::CommandToken & token, const Executor& exe) :exe_(exe)
      {
        token.GET(layer).GET(page).GET(left).GET(top).GET(width).GET(height)
          .GET(marginl).GET(margint).GET(marginr).GET(marginb)
          .GET(color).GET(frame).GET(opacity).GET(visible);
        if ( layer.first != converter::LayerType::Message )
          throw std::runtime_error("position");
      }
      std::pair<converter::LayerType, int> layer;
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
        MessageLayer ptr = manager.GetLayer(layer.second == Define::default ? 0 : layer.second,
                                            page ? *page : manager.CurrentPage());
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
    };
    executor_.Command([tag = PositionVal(token, executor_)]() { tag.attach(); });
  }

  void FileExecutor::Pimpl::CHTag(Parser::CommandToken & token)
  {
    Parser::Must<SnapShotSpan> text;
    token.GET(text);
    executor_.CommandTextNoDelay(*text);
  }

}
