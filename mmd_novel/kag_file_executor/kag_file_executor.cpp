#include "kag_file_executor.h"
namespace kag {



  FileExecutor::FileExecutor() {
    //tag_func_[SnapShotSpan(L"cancelautomode")] = &FileExecutor::CancelAutoModeTag;
    //tag_func_[SnapShotSpan(L"cancelskip")] = &FileExecutor::CancelSkipTag;
    tag_func_[SnapShotSpan(L"ch")] = &FileExecutor::CHTag;
    tag_func_[SnapShotSpan(L"cm")] = &FileExecutor::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &FileExecutor::CTTag;
    tag_func_[SnapShotSpan(L"current")] = &FileExecutor::CurrentTag;
    tag_func_[SnapShotSpan(L"deffont")] = &FileExecutor::DefFontTag;
    //tag_func_[SnapShotSpan(L"defstyle")] = &FileExecutor::DefStyleTag;
    tag_func_[SnapShotSpan(L"delay")] = &FileExecutor::DelayTag;
    tag_func_[SnapShotSpan(L"endindent")] = &FileExecutor::EndIndentTag;
    tag_func_[SnapShotSpan(L"endnowait")] = &FileExecutor::EndNoWaitTag;
    tag_func_[SnapShotSpan(L"er")] = &FileExecutor::ERTag;
    tag_func_[SnapShotSpan(L"font")] = &FileExecutor::FontTTag;
    //tag_func_[SnapShotSpan(L"glyph")] = &FileExecutor::GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &FileExecutor::GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &FileExecutor::HCH;
    tag_func_[SnapShotSpan(L"indent")] = &FileExecutor::IndentTag;
    tag_func_[SnapShotSpan(L"l")] = &FileExecutor::LTag;
    //tag_func_[SnapShotSpan(L"locate")] = &FileExecutor::LocateTag;
    //tag_func_[SnapShotSpan(L"locklink")] = &FileExecutor::LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = &FileExecutor::NoWaitTag;
    tag_func_[SnapShotSpan(L"p")] = &FileExecutor::PTag;
    tag_func_[SnapShotSpan(L"position")] = &FileExecutor::PositionTTag;
    tag_func_[SnapShotSpan(L"r")] = &FileExecutor::RTag;
    tag_func_[SnapShotSpan(L"resetfont")] = &FileExecutor::ResetFontTag;
    //tag_func_[SnapShotSpan(L"resetstyle")] = &FileExecutor::ResetStyleTag;
    //tag_func_[SnapShotSpan(L"ruby")] = &FileExecutor::RubyTag;
    //tag_func_[SnapShotSpan(L"style")] = &FileExecutor::StyleTag;
    //tag_func_[SnapShotSpan(L"unlocklink")] = &FileExecutor::UnlockLinkTag;


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

  void FileExecutor::ResetFontTag(const Parser::CommandToken & token) {
    CommandResetFont();
  }

  void FileExecutor::NoWaitTag(const Parser::CommandToken & token) {
    CommandNoWait();
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
        CommandNoWait();
      } else {
        CommandDelay(converter::ToInt10(val));
      }
    }, []() {
      throw std::runtime_error("[delay] にはspeed属性が必須");
    });

  }

  void FileExecutor::EndIndentTag(const Parser::CommandToken & token) {
    CommandEndIndent();
  }

  void FileExecutor::EndNoWaitTag(const Parser::CommandToken & token) {
    CommandEndNoWait();
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

  void FileExecutor::CurrentTag(const Parser::CommandToken & token) {
    int layer = kag::default;
    int page = 0;
    auto& args = token.arguments();
    using namespace converter;
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { layer = val; });
    args.AttributeValTo(L"page", ToPageNum, [&](int val) { page = val; });
    CommandCurrent(layer, page);
  }

  void FileExecutor::DefFontTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    CommandDefFont([args](FontCommandEditor& editor) {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

    });
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

  void FileExecutor::IndentTag(const Parser::CommandToken & token) {
    CommandIndent();
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

      args.AttributeValTo(L"visible", ToBool, [&](bool val) { editor.visible(val); });

    });
  }

  void FileExecutor::CHTag(const Parser::CommandToken & token) {
    CommandTextNoDelay(token.arguments().find_or_throw(L"text"));
  }

}
