#include "kag_file_executor.h"
namespace kag {



  FileExecutor::FileExecutor() {
    tag_func_[SnapShotSpan(L"l")] = &FileExecutor::LTag;
    tag_func_[SnapShotSpan(L"r")] = &FileExecutor::RTag;
    tag_func_[SnapShotSpan(L"p")] = &FileExecutor::PTag;
    tag_func_[SnapShotSpan(L"delay")] = &FileExecutor::DelayTag;
    tag_func_[SnapShotSpan(L"er")] = &FileExecutor::ERTag;
    tag_func_[SnapShotSpan(L"cm")] = &FileExecutor::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &FileExecutor::CTTag;
    tag_func_[SnapShotSpan(L"font")] = &FileExecutor::FontTTag;
    tag_func_[SnapShotSpan(L"position")] = &FileExecutor::PositionTTag;
  }

  FileExecutor::FileExecutor(const FilePath & filename) :FileExecutor() {
    parser_ = Parser(filename);
  }

  void FileExecutor::Update() {
    if (!Executor::Update()) return;

    while (!is_wait_click_) {
      switch (parser_.nextType()) {
      case  kag::Parser::Type::Text:
        CommandText(parser_.readText());
        return;

      case  kag::Parser::Type::Command:
      {
        auto token = parser_.readCommand();
        auto func = tag_func_[token.name()];
        if (func) {
          (this->*func)(token);
        } else {
          throw std::runtime_error(token.name().ToNarrow());
        }
        break;
      }

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
  タグの実装
  - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void FileExecutor::LTag(const Parser::CommandToken &) {
    CommandL();
  }

  void FileExecutor::RTag(const Parser::CommandToken &) {
    CommandR();
  }

  void FileExecutor::PTag(const Parser::CommandToken &) {
    CommandP();
  }


  void FileExecutor::DelayTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    auto res = args.AttributeValTo<SnapShotSpan>(L"speed");
    if (res.result == FindAttributeResult::Ok) {
      auto i = ToCast<int>(*res.val);
      CommandDelay(*i);
    } else {
      ShowErrorMsg(L"[delay] にはspeed属性が必須");
    }
  }

  void FileExecutor::ERTag(const Parser::CommandToken &) {
    CommandER();
  }

  void FileExecutor::CMTag(const Parser::CommandToken &) {
    CommandCM();
  }

  void FileExecutor::CTTag(const Parser::CommandToken &) {
    CommandCT();
  }

  void FileExecutor::FontTTag(const Parser::CommandToken & token) {

    auto& args = token.arguments();
    auto editor = CommandFont();

    args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

    args.AttributeVal(L"size", [&](const SnapShotSpan& val) { editor.size(*val.ToInt()); });

    args.AttributeVal(L"italic", [&](const SnapShotSpan& val) { editor.is_italic(val == L"true"); });

    args.AttributeVal(L"bold", [&](const SnapShotSpan& val) { editor.is_bold(val == L"true"); });

    editor.Commit();
  }

  void FileExecutor::PositionTTag(const Parser::CommandToken & token) {
    int index = kag::default;
    int page = kag::default;
    auto& args = token.arguments();
    args.AttributeVal(L"layer", [&](const SnapShotSpan& val) {
      index = _wtoi(&val[7]);
    });

    args.AttributeVal(L"page", [&](const SnapShotSpan& val) {
      page = val == L"back";
    });

    auto editor = CommandPosition(index, page);

    args.AttributeValTo<int>(L"left", [&](int val) { editor.position_left(val); });

    args.AttributeValTo<int>(L"top", [&](int val) { editor.position_top(val); });

    args.AttributeValTo<int>(L"width", [&](int val) { editor.position_width(val); });

    args.AttributeValTo<int>(L"height", [&](int val) { editor.position_height(val); });

    args.AttributeValTo<int>(L"marginl", [&](int val) { editor.margin_left(val); });

    args.AttributeValTo<int>(L"margint", [&](int val) { editor.margin_top(val); });

    args.AttributeValTo<int>(L"marginr", [&](int val) { editor.margin_right(val); });

    args.AttributeValTo<int>(L"marginb", [&](int val) { editor.margin_bottom(val); });

    editor.Commit();
  }

}