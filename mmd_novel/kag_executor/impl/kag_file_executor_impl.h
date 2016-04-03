#pragma once
#include <MmdNovel/kag_file_executor.h>
#include <kag_parser/kag_parser.h>
namespace kag {
  class FileExecutor::Pimpl {
  public:
    Pimpl(const Executor& exe);
    void Load(const FilePath& path);
    void Update();
  private:
    Executor executor_;

    Parser parser_;
    using TagFunction = void(FileExecutor::Pimpl::*)(const kag::Parser::CommandToken&);
    std::map<SnapShotSpan, TagFunction> tag_func_;

    //タグリファレンス
    //http://devdoc.kikyou.info/tvp/docs/kag3doc/contents/index.html

    //メッセージ関連

    //void CancelAutoModeTag(const Parser::CommandToken& token);
    //void CancelSkipTag(const Parser::CommandToken& token);
    void CHTag(const Parser::CommandToken& token);
    void CMTag(const Parser::CommandToken& token);
    void CTTag(const Parser::CommandToken& token);
    void CurrentTag(const Parser::CommandToken& token);
    void DefFontTag(const Parser::CommandToken& token);
    void DefStyleTag(const Parser::CommandToken& token);
    void DelayTag(const Parser::CommandToken& token);
    void EndIndentTag(const Parser::CommandToken& token);
    void EndNoWaitTag(const Parser::CommandToken& token);
    void ERTag(const Parser::CommandToken& token);
    void FontTTag(const Parser::CommandToken& token);
    //void GlyphTag(const Parser::CommandToken& token);
    //void GraphTag(const Parser::CommandToken& token);
    //void HCHTag(const Parser::CommandToken& token);
    void IndentTag(const Parser::CommandToken& token);
    void LTag(const Parser::CommandToken& token);
    void LocateTag(const Parser::CommandToken& token);
    //void LockLinkTag(const Parser::CommandToken& token);
    void NoWaitTag(const Parser::CommandToken& token);
    void PTag(const Parser::CommandToken& token);
    void PositionTTag(const Parser::CommandToken& token);
    void RTag(const Parser::CommandToken& token);
    void ResetFontTag(const Parser::CommandToken& token);
    void ResetStyleTag(const Parser::CommandToken& token);
    //void RubyTag(const Parser::CommandToken& token);
    void StyleTag(const Parser::CommandToken& token);
    //void UnlockLinkTag(const Parser::CommandToken& token);

  };


  FileExecutor::Pimpl::Pimpl(const Executor& exe) :executor_(exe) {
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


  }

  void FileExecutor::Pimpl::Load(const FilePath & path) {
    parser_ = Parser(path);
  }

  void FileExecutor::Pimpl::Update() {
    if (!executor_.Update()) return;

    while (executor_.CommandUpdate()) {
      switch (parser_.nextType()) {
      case  kag::Parser::Type::Text:
        executor_.CommandText(parser_.readText());
        break;

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
  void FileExecutor::Pimpl::LTag(const Parser::CommandToken &) {
    executor_.CommandL();
  }

  void FileExecutor::Pimpl::LocateTag(const Parser::CommandToken & token) {
    int x = kag::default;
    int y = kag::default;
    auto& args = token.arguments();
    using namespace converter;
    args.AttributeValTo(L"x", ToInt10, [&](int val) { x = val; });
    args.AttributeValTo(L"y", ToInt10, [&](int val) { y = val; });

    executor_.CommandLocate(x, y);
  }

  void FileExecutor::Pimpl::RTag(const Parser::CommandToken &) {
    executor_.CommandR();
  }

  void FileExecutor::Pimpl::ResetFontTag(const Parser::CommandToken & token) {
    executor_.CommandResetFont();
  }

  void FileExecutor::Pimpl::ResetStyleTag(const Parser::CommandToken & token) {
    executor_.CommandResetStyle();
  }

  void FileExecutor::Pimpl::StyleTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandStyle([=](StyleCommandEditor& editor) {
      args.AttributeVal(L"linesize", [&](const SnapShotSpan& val) {
        if (val == L"default") {
          editor.linesize();
        } else {
          editor.linesize(converter::ToInt10(val));
        }
      });

      args.AttributeValTo(L"linespacing", converter::ToInt10, [&](int val) {
        editor.linespacing(val);
      });

    });
  }

  void FileExecutor::Pimpl::NoWaitTag(const Parser::CommandToken & token) {
    executor_.CommandNoWait();
  }

  void FileExecutor::Pimpl::PTag(const Parser::CommandToken &) {
    executor_.CommandP();
  }

  void FileExecutor::Pimpl::DelayTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    args.AttributeVal(L"speed", [&](const SnapShotSpan& val) {
      if (val == L"user") {
        executor_.CommandDelay(30);
      } else if (val == L"nowait") {
        executor_.CommandNoWait();
      } else {
        executor_.CommandDelay(converter::ToInt10(val));
      }
    }, []() {
      throw std::runtime_error("[delay] にはspeed属性が必須");
    });

  }

  void FileExecutor::Pimpl::EndIndentTag(const Parser::CommandToken & token) {
    executor_.CommandEndIndent();
  }

  void FileExecutor::Pimpl::EndNoWaitTag(const Parser::CommandToken & token) {
    executor_.CommandEndNoWait();
  }

  void FileExecutor::Pimpl::ERTag(const Parser::CommandToken &) {
    executor_.CommandER();
  }

  void FileExecutor::Pimpl::CMTag(const Parser::CommandToken &) {
    executor_.CommandCM();
  }

  void FileExecutor::Pimpl::CTTag(const Parser::CommandToken &) {
    executor_.CommandCT();
  }

  void FileExecutor::Pimpl::CurrentTag(const Parser::CommandToken & token) {
    int layer = kag::default;
    int page = 0;
    auto& args = token.arguments();
    using namespace converter;
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { layer = val; });
    args.AttributeValTo(L"page", ToPageNum, [&](int val) { page = val; });
    executor_.CommandCurrent(layer, page);
  }

  void FileExecutor::Pimpl::DefFontTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefFont([args](FontCommandEditor& editor) {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

    });
  }

  void FileExecutor::Pimpl::DefStyleTag(const Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefStyle([=](DefaultStyleCommandEditor& editor) {
      args.AttributeVal(L"linesize", [&](const SnapShotSpan& val) {
        if (val == L"default") {
          editor.linesize();
        } else {
          editor.linesize(converter::ToInt10(val));
        }
      });

      args.AttributeValTo(L"linespacing", converter::ToInt10, [&](int val) {
        editor.linespacing(val);
      });

    });
  }

  void FileExecutor::Pimpl::FontTTag(const Parser::CommandToken & token) {

    auto& args = token.arguments();
    executor_.CommandFont([args](FontCommandEditor& editor) {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

      args.AttributeValTo(L"color", ToColor, [&](const Color& val) {editor.color(val); });

      args.AttributeValTo(L"shadow", ToBool, [&](bool val) {editor.is_shadow(val); });

      args.AttributeValTo(L"shadowcolor", ToColor, [&](const Color& val) { editor.shadowcolor(val); });
    });

  }

  void FileExecutor::Pimpl::IndentTag(const Parser::CommandToken & token) {
    executor_.CommandIndent();
  }

  void FileExecutor::Pimpl::PositionTTag(const Parser::CommandToken & token) {
    using namespace converter;
    int index = kag::default;
    int page = kag::default;
    auto& args = token.arguments();
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { index = val; });

    args.AttributeValTo(L"page", ToPageNum, [&](int val) { page = val; });

    executor_.CommandPosition(index, page, [args](PositionCommandEditor& editor) {
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

  void FileExecutor::Pimpl::CHTag(const Parser::CommandToken & token) {
    executor_.CommandTextNoDelay(token.arguments().find_or_throw(L"text"));
  }

}
