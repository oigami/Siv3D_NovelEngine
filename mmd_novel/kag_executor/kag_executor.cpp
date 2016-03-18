#include "kag_executor.h"
namespace kag {
  Executor::Executor(const FilePath & path) :parser_(path) {
    tag_func_[SnapShotSpan(L"l")] = &Executor::LTag;
    tag_func_[SnapShotSpan(L"r")] = &Executor::RTag;
    tag_func_[SnapShotSpan(L"p")] = &Executor::PTag;
    tag_func_[SnapShotSpan(L"delay")] = &Executor::DelayTag;
    tag_func_[SnapShotSpan(L"er")] = &Executor::ERTag;
    tag_func_[SnapShotSpan(L"cm")] = &Executor::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &Executor::CTTag;
    tag_func_[SnapShotSpan(L"font")] = &Executor::FontTTag;
    tag_func_[SnapShotSpan(L"position")] = &Executor::PositionTTag;
    Clear();
  }
  void Executor::Clear() {
    is_wait_click_ = false;
    is_click_new_page = false;
    message_manager_.Clear();
  }

  void Executor::Update() {
    if (!message_manager_.Update()) {
      if (Input::MouseL.clicked)
        message_manager_.NextPage();
      return;
    } else if (message_manager_.IsFlush() == false) {
      if (Input::MouseL.clicked)
        message_manager_.Flush();
      return;
    }
    if (is_wait_click_) {
      if (!Input::MouseL.clicked)
        return;
      is_wait_click_ = false;
      if (is_click_new_page) {
        is_click_new_page = false;
        message_manager_.NextPage();
      }
    }

    while (!is_wait_click_) {
      switch (parser_.nextType()) {
      case  kag::Parser::Type::Text:
        message_manager_.Append(parser_.readText());
        return;

      case  kag::Parser::Type::Command:
      {
        auto token = parser_.readCommand();
        auto func = tag_func_[token.name()];
        if (func) {
          (this->*func)(token);
        }
        break;
      }

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  void Executor::Draw() {
    message_manager_.Draw();
  }



  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
                                      タグの実装
   - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void Executor::LTag(const Parser::CommandToken &) {
    is_wait_click_ = true;
  }

  void Executor::RTag(const Parser::CommandToken &) {
    message_manager_.AppendNewLine();
  }

  void Executor::PTag(const Parser::CommandToken &) {
    is_wait_click_ = true;
  }


  void Executor::DelayTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    auto res = args.AttributeValTo<SnapShotSpan>(L"speed");
    if (res.result == FindAttributeResult::Ok) {
      auto i = ToCast<int>(*res.val);
      message_manager_.SetDelayTime(*i);
    } else {
      ShowErrorMsg(L"[delay] にはspeed属性が必須");
    }
  }

  void Executor::ERTag(const Parser::CommandToken & token) {
    message_manager_.Clear();
  }

  void Executor::CMTag(const Parser::CommandToken & token) {
    message_manager_.AllClear();
  }

  void Executor::CTTag(const Parser::CommandToken & token) {
    CMTag(token);
    message_manager_.SetCurrent(0, LayerPage::Fore);
  }

  void Executor::FontTTag(const Parser::CommandToken & token) {
    FontProperty prop;

    auto& args = token.arguments();
    auto& current = message_manager_.Current();
    auto& now_font = current.NowFont();
    bool is_italic;
    bool is_bold;
    switch (now_font.font_.style()) {
    case s3d::FontStyle::BitmapBoldItalic:
    case s3d::FontStyle::BoldItalic:
      is_italic = true;
    case s3d::FontStyle::BitmapBold:
    case s3d::FontStyle::Bold:
      is_bold = true;
      break;
    case s3d::FontStyle::BitmapItalic:
    case s3d::FontStyle::Italic:
      is_italic = true;
    default:
      is_italic = false;
      is_bold = false;
      break;
    }

    args.AttributeValTo<SnapShotSpan>(L"face", [&](const SnapShotSpan& val) {
      prop.name = val.ToStr();
    }, [&](auto err) {
      prop.name = now_font.font_.name();
    });

    args.AttributeValTo<SnapShotSpan>(L"size", [&](const SnapShotSpan& val) {
      prop.size = *val.ToInt();
    });

    args.AttributeValTo<SnapShotSpan>(L"italic", [&](const SnapShotSpan& val) {
      is_italic = val == L"true";
    });

    args.AttributeValTo<SnapShotSpan>(L"bold", [&](const SnapShotSpan& val) {
      is_bold = val == L"true";
    });
    int style_cnt = (is_italic == true) + (is_bold == true) * 2;
    switch (style_cnt) {
    case 0: prop.style = FontStyle::Regular;    break;
    case 1: prop.style = FontStyle::Italic;     break;
    case 2: prop.style = FontStyle::Bold;       break;
    case 3: prop.style = FontStyle::BoldItalic; break;
    }

    message_manager_.Current().SetFont({ Font(prop),Palette::White });

  }

  void Executor::PositionTTag(const Parser::CommandToken & token) {
    int index;
    int page;

    auto& args = token.arguments();
    args.AttributeValTo<SnapShotSpan>(L"layer", [&](const SnapShotSpan& val) {
      index = _wtoi(&val[7]);
    }, [&](const auto&) {
      index = message_manager_.CurrentLayerNum();
    });

    args.AttributeValTo<SnapShotSpan>(L"page", [&](const SnapShotSpan& val) {
      page = val == L"back";
    }, [&](const auto&) {
      page = message_manager_.CurrentPageNum();
    });

    auto& target = message_manager_.GetLayer(index, page);

    args.AttributeValTo<int>(L"left", [&](int val) {
      target.SetPositionLeft(val);
    });

    args.AttributeValTo<int>(L"top", [&](int val) {
      target.SetPositionTop(val);
    });

    args.AttributeValTo<int>(L"width", [&](int val) {
      target.SetPositionWidth(val);
    });

    args.AttributeValTo<int>(L"height", [&](int val) {
      target.SetPositionHeight(val);
    });

    args.AttributeValTo<int>(L"marginl", [&](int val) {
      target.SetMarginLeft(val);
    });

    args.AttributeValTo<int>(L"margint", [&](int val) {
      target.SetMarginTop(val);
    });

    args.AttributeValTo<int>(L"marginr", [&](int val) {
      target.SetMarginRight(val);
    });

    args.AttributeValTo<int>(L"marginb", [&](int val) {
      target.SetMarginBottom(val);
    });

    target.Clear();

  }

}
