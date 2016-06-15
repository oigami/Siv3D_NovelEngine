#pragma once
#include <MmdNovel/message/message_manager.h>
#define GET(name) get(L#name, name)
namespace kag
{
  void MessageManager::AddTag(IFileManager::FuncList& tag_func_)
  {
    auto bind = [this](auto func) { return [=](CommandToken& token) { return (this->*func)(token); }; };
    tag_func_[SnapShotSpan(L"ch")] = bind(&MessageManager::CHTag);
    tag_func_[SnapShotSpan(L"cm")] = bind(&MessageManager::CMTag);
    tag_func_[SnapShotSpan(L"ct")] = bind(&MessageManager::CTTag);
    tag_func_[SnapShotSpan(L"current")] = bind(&MessageManager::CurrentTag);
    tag_func_[SnapShotSpan(L"deffont")] = bind(&MessageManager::DefFontTag);
    tag_func_[SnapShotSpan(L"defstyle")] = bind(&MessageManager::DefStyleTag);
    tag_func_[SnapShotSpan(L"delay")] = bind(&MessageManager::DelayTag);
    tag_func_[SnapShotSpan(L"endindent")] = bind(&MessageManager::EndIndentTag);
    tag_func_[SnapShotSpan(L"endnowait")] = bind(&MessageManager::EndNoWaitTag);
    tag_func_[SnapShotSpan(L"er")] = bind(&MessageManager::ERTag);
    tag_func_[SnapShotSpan(L"font")] = bind(&MessageManager::FontTTag);

    //tag_func_[SnapShotSpan(L"glyph")] = &GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &HCH;
    tag_func_[SnapShotSpan(L"indent")] = bind(&MessageManager::IndentTag);
    tag_func_[SnapShotSpan(L"l")] = bind(&MessageManager::LTag);
    tag_func_[SnapShotSpan(L"locate")] = bind(&MessageManager::LocateTag);

    //tag_func_[SnapShotSpan(L"locklink")] = &LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = bind(&MessageManager::NoWaitTag);
    tag_func_[SnapShotSpan(L"p")] = bind(&MessageManager::PTag);
    tag_func_[SnapShotSpan(L"position")] = bind(&MessageManager::PositionTTag);
    tag_func_[SnapShotSpan(L"r")] = bind(&MessageManager::RTag);
    tag_func_[SnapShotSpan(L"resetfont")] = bind(&MessageManager::ResetFontTag);
    tag_func_[SnapShotSpan(L"resetstyle")] = bind(&MessageManager::ResetStyleTag);

    //tag_func_[SnapShotSpan(L"ruby")] = &RubyTag;
    tag_func_[SnapShotSpan(L"style")] = bind(&MessageManager::StyleTag);
  }
  MessageManager::MessageManager(const Executor& exe) :IFileManager(exe), current_page_(LayerPage::Fore)
  {
    click_key_ = Input::MouseL | Input::KeyEnter;
    is_active_key_ = true;
    resize(2);
    message_layer_[0][LayerPage::Fore]->IsVisible(true);
    delay_time_ = 30;
    delay_index_ = 0;
    current_layer_ = 0;
    is_no_wait_ = false;
  }

  void MessageManager::resize(size_t size)
  {
    message_layer_.resize(size);
  }

  int MessageManager::size() const
  {
    return static_cast<int>(message_layer_.size());
  }

  void MessageManager::Clear()
  {
    is_wait_click_ = false;
    is_click_new_page_ = false;
    message_layer_[current_layer_][current_page_]->Clear();
  }

  void MessageManager::AllClear()
  {
    for ( auto& i : message_layer_ )
    {
      i[LayerPage::Fore]->Clear();
      i[LayerPage::Back]->Clear();
    }
  }

  bool MessageManager::IsFlush() const
  {
    return delay_text_.Length() == delay_index_;
  }

  void MessageManager::Flush()
  {
    auto& current = Current();
    for ( int i = 0, len = delay_text_.Length() - delay_index_; i < len; i++ )
    {
      current->Append(delay_text_[delay_index_++]);
      if ( current->IsLimitHeihgt() )
      {
        is_click_new_page_ = true;
        is_wait_click_ = true;
        return;
      }
    }
  }

  void MessageManager::Append(const SnapShotSpan & text)
  {
    assert(IsFlush());
    if ( is_no_wait_ )
    {
      auto& current = Current();
      current->Append(text.ToStr());
      if ( current->IsLimitHeihgt() )
      {
        is_click_new_page_ = true;
        is_wait_click_ = true;
        return;
      }
    }
    else
    {
      delay_text_ = text;
      timer_.restart();
      delay_index_ = 0;
    }
  }

  void MessageManager::AppendNewLine()
  {
    Current()->AppenNewLine();
  }

  void MessageManager::NextPage()
  {
    Current()->NextPage();
    timer_.set(MillisecondsF(delay_time_ * delay_index_));
  }

  void MessageManager::SetWaitClick() { is_wait_click_ = true; }

  void MessageManager::SetClickNextPage() { is_click_new_page_ = true; }

  bool MessageManager::Update()
  {
    if ( is_wait_click_ )
    {
      if ( CheckClicked() )
      {
        if ( is_click_new_page_ )
          NextPage();
        is_click_new_page_ = false;
        is_wait_click_ = false;
      }
      return false;
    }
    auto& current = Current();
    int ms = timer_.ms();
    int loop = ms / delay_time_ - delay_index_;
    loop = std::min(loop, delay_text_.Length() - delay_index_);
    for ( int i = 0; i < loop; i++ )
    {
      current->Append(delay_text_[delay_index_++]);
      if ( current->IsLimitHeihgt() )
      {
        is_click_new_page_ = true;
        is_wait_click_ = true;
        return false;
      }
    }

    if ( IsFlush() == false )
    {
      if ( CheckClicked() )
        Flush();
      return false;
    }

    return true;
  }

  void MessageManager::Draw() const
  {
    for ( auto& i : message_layer_ )
    {
      i.Draw();
    }
  }

  void MessageManager::SetCurrentLayer(int layer_index)
  {
    current_layer_ = layer_index;
  }

  void MessageManager::SetCurrent(int layer_index, LayerPage type)
  {
    current_layer_ = layer_index;
    current_page_ = type;
  }

  void MessageManager::SetDelayTime(int delay_time)
  {
    delay_time_ = delay_time;
  }

  void MessageManager::SetNoWaitText(bool is_no_wait)
  {
    is_no_wait_ = is_no_wait;
  }

  MessageLayer & MessageManager::GetLayer(int index, LayerPage page)
  {
    return message_layer_[index][page];
  }

  PageLayer<MessageLayer> & MessageManager::GetLayer(int index)
  {
    return message_layer_[index];
  }

  MessageLayer & MessageManager::Current() { return GetLayer(current_layer_, current_page_); }

  int MessageManager::CurrentLayerNum() const { return current_layer_; }

  LayerPage MessageManager::CurrentPage() const { return current_page_; }

  void MessageManager::SetClickKey(const KeyCombination & key)
  {
    click_key_ = key;
  }

  void MessageManager::SetInvalidKeyInput() { is_active_key_ = false; }

  void MessageManager::SetValidKeyInput() { is_active_key_ = true; }

  void MessageManager::SetLayerManager(LayerManager& manager)
  {
    layer_manager_ = manager;
    for ( auto& i : message_layer_ )
    {
      layer_manager_->Set(i);
    }
  }

  bool MessageManager::IsWait() const { return is_wait_click_ || !IsFlush(); }

  bool MessageManager::CheckClicked() const
  {
    if ( is_active_key_ ) return click_key_.clicked;
    return false;
  }

  void MessageManager::NoWaitTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->SetNoWaitText(true);
    });
  }

  void MessageManager::PTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->SetWaitClick();
    });
  }

  void MessageManager::DelayTag(CommandToken & token)
  {
    Must<SnapShotSpan> speed;
    if ( token.GET(speed).HasError() ) return;

    if ( *speed == L"user" )
    {
      executor_.Command([manager = shared_from_this()]()
      {
        manager->SetDelayTime(30);
      });
    }
    else if ( *speed == L"nowait" )
    {
      executor_.Command([manager = shared_from_this()]()
      {
        manager->SetNoWaitText(true);
      });
    }
    else
    {
      int val;
      if ( converter::TryConvert(*speed, val) )
      {
        executor_.Command([val, manager = shared_from_this()]()
        {
          manager->SetDelayTime(val);
        });
      }
      else
      {
        token.AddIllegalException(L"speed", *speed);
      }
    }

  }

  void MessageManager::EndIndentTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->Current()->EndIndent();
    });
  }

  void MessageManager::EndNoWaitTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->SetNoWaitText(true);
    });
  }

  void MessageManager::ERTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->Clear();
    });
  }

  void MessageManager::CMTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->AllClear();
    });
  }

  void MessageManager::CTTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->AllClear();
      manager->SetCurrent(0, LayerPage::Fore);
    });
  }

  void MessageManager::CurrentTag(CommandToken & token)
  {
    std::pair<converter::LayerType, int> layer = { L"message", Define::default };
    LayerPage page = LayerPage::Fore;

    using namespace converter;
    if ( token.GET(layer).GET(page).HasError() )
      return;
    executor_.Command([layer, page, manager = shared_from_this()]()
    {
      manager->SetCurrent(layer.second, page);
    });
  }

  namespace
  {
    struct FontVal
    {
      std::shared_ptr<MessageManager> exe_;
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
        auto now_font = exe_->Current()->NowFont();
        FontProperty prop;
        prop.name = face ? face->ToStr() : now_font.font_.name();
        prop.size = size ? *size : now_font.font_.size();
        prop.style = CreateStyle();
        message::TextFont new_font = now_font;
        new_font.font_ = Font(prop);
        if ( color ) new_font.color_ = *color;
        if ( shadow ) new_font.is_shadow_ = *shadow;
        if ( shadowcolor ) new_font.shadow_color_ = *shadowcolor;
        commit(exe_->Current(), new_font);
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
      FontVal(CommandToken& token, const std::shared_ptr<MessageManager>& exe)
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

  void MessageManager::DefFontTag(CommandToken & token)
  {
    DefFont tag(token, shared_from_this());
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

  void MessageManager::DefStyleTag(CommandToken & token)
  {
    struct DefStyleVal
    {
      DefStyleVal(CommandToken & token, const std::shared_ptr<MessageManager>& exe)
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
        exe_->Current()->SetDefaultStyle(style);
      }
      int linesize;
      int linespacing;
      int pitch;
      std::shared_ptr<MessageManager> exe_;
    };
    executor_.Command([tag = DefStyleVal(token, shared_from_this())](){ tag.attach(); });
  }

  void MessageManager::FontTTag(CommandToken & token)
  {
    executor_.Command([tag = FontVal(token, shared_from_this())]() { tag.attach(); });
  }

  void MessageManager::IndentTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->Current()->BeginIndent();
    });
  }

  void MessageManager::PositionTTag(CommandToken & token)
  {
    struct PositionVal
    {
      PositionVal(CommandToken & token, const std::shared_ptr<MessageManager>& exe) :exe_(exe)
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
      std::shared_ptr<MessageManager> exe_;
      void attach()const
      {
        int start = layer ? layer->second : exe_->CurrentLayerNum();
        int end = start + 1;
        if ( start == Define::default )
        {
          end = exe_->size();
          start = 0;
        }
        const LayerPage p = page ? *page : exe_->CurrentPage();
        for ( int i = start; i < end; i++ )
        {
          MessageLayer ptr = exe_->GetLayer(i, p);
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
    PositionVal tag(token, shared_from_this());
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }
  void MessageManager::CHTag(CommandToken & token)
  {
    Must<SnapShotSpan> text;
    if ( token.GET(text).HasError() ) return;
    executor_.Command([manager = shared_from_this(), text]() mutable
    {
      manager->Current()->Append(text->ToStr());
    });
  }
  void MessageManager::LTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->SetWaitClick();
    });
  }

  void MessageManager::LocateTag(CommandToken & token)
  {
    Value<int> x, y;
    if ( token.GET(x).GET(y).HasError() ) return;
    executor_.Command([x, y, manager = shared_from_this()]()
    {
      if ( x == Define::default )
      {
        manager->Current()->SetLocateY(y());
      }
      else if ( y == Define::default )
      {
        manager->Current()->SetLocateX(x());
      }
      else
      {
        manager->Current()->SetLocate(x(), y());
      }
    });
  }

  void MessageManager::RTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->AppendNewLine();
    });
  }

  void MessageManager::ResetFontTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->Current()->ResetFont();
    });
  }

  void MessageManager::ResetStyleTag(CommandToken &)
  {
    executor_.Command([manager = shared_from_this()]()
    {
      manager->Current()->ResetStyle();
    });
  }

  void MessageManager::StyleTag(CommandToken & token)
  {
    struct StyleVal
    {
      Optional<int> linesize_;
      Optional<int> linespacing;

      std::shared_ptr<MessageManager> exe_;
      StyleVal(CommandToken& token, const std::shared_ptr<MessageManager>& exe) : exe_(exe)
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
        auto& layer = exe_->Current();
        if ( linesize_ ) layer->SetLineSize(*linesize_);
      }
    };
    StyleVal tag(token, shared_from_this());
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

}
