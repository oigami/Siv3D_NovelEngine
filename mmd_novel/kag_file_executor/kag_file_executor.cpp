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

    while (CommandUpdate()) {
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
    args.AttributeVal(L"speed", [&](const SnapShotSpan& val) {
      if (val == L"user") {
        CommandDelay(30);
      } else if (val == L"nowait") {
        CommandDelay(0);
      } else {
        CommandDelay(converter::ToInt10(val));
      }
    }, []() {
      throw std::runtime_error("[delay] にはspeed属性が必須");
    });

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
    CommandFont([args](FontCommandEditor& editor) {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

    });

  }

  void FileExecutor::PositionTTag(const Parser::CommandToken & token) {
    using namespace converter;
    int index = kag::default;
    int page = kag::default;
    auto& args = token.arguments();
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { index = val; });

    args.AttributeValTo(L"page", ToPageNum, [&](int val) { page = val; });

    CommandPosition(index, page, [args](PositionCommandEditor& editor) {
      args.AttributeValTo(L"left", ToInt10, [&](int val) { editor.position_left(val); });

      args.AttributeValTo(L"top", ToInt10, [&](int val) { editor.position_top(val); });

      args.AttributeValTo(L"width", ToInt10, [&](int val) { editor.position_width(val); });

      args.AttributeValTo(L"height", ToInt10, [&](int val) { editor.position_height(val); });

      args.AttributeValTo(L"marginl", ToInt10, [&](int val) { editor.margin_left(val); });

      args.AttributeValTo(L"margint", ToInt10, [&](int val) { editor.margin_top(val); });

      args.AttributeValTo(L"marginr", ToInt10, [&](int val) { editor.margin_right(val); });

      args.AttributeValTo(L"marginb", ToInt10, [&](int val) { editor.margin_bottom(val); });

      args.AttributeValTo(L"color", ToColor, [&](const Color& col) {
        editor.color(col.r, col.g, col.b);
      });

      args.AttributeVal(L"frame", [&](const SnapShotSpan& val) {
        if (val.Length() != 0) {
          editor.frame(Texture(val.ToStr()));
        } else {
          editor.frame(Texture());
        }
      });

      args.AttributeValTo(L"opacity", ToInt10, [&](int val) { editor.opacity(val); });

      args.AttributeValTo(L"visible", ToBool, [&](bool val) { editor.visible(val); })
    });
  }

}