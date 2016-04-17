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
    using TagFunction = void(FileExecutor::Pimpl::*)(Parser::CommandToken&);
    std::map<SnapShotSpan, TagFunction> tag_func_;

    //タグリファレンス
    //http://devdoc.kikyou.info/tvp/docs/kag3doc/contents/index.html

    //メッセージ関連

    //void CancelAutoModeTag(Parser::CommandToken& token);
    //void CancelSkipTag(Parser::CommandToken& token);
    void CHTag(Parser::CommandToken& token);
    void CMTag(Parser::CommandToken& token);
    void CTTag(Parser::CommandToken& token);
    void CurrentTag(Parser::CommandToken& token);
    void DefFontTag(Parser::CommandToken& token);
    void DefStyleTag(Parser::CommandToken& token);
    void DelayTag(Parser::CommandToken& token);
    void EndIndentTag(Parser::CommandToken& token);
    void EndNoWaitTag(Parser::CommandToken& token);
    void ERTag(Parser::CommandToken& token);
    void FontTTag(Parser::CommandToken& token);
    //void GlyphTag(Parser::CommandToken& token);
    //void GraphTag(Parser::CommandToken& token);
    //void HCHTag(Parser::CommandToken& token);
    void IndentTag(Parser::CommandToken& token);
    void LTag(Parser::CommandToken& token);
    void LocateTag(Parser::CommandToken& token);
    //void LockLinkTag(Parser::CommandToken& token);
    void NoWaitTag(Parser::CommandToken& token);
    void PTag(Parser::CommandToken& token);
    void PositionTTag(Parser::CommandToken& token);
    void RTag(Parser::CommandToken& token);
    void ResetFontTag(Parser::CommandToken& token);
    void ResetStyleTag(Parser::CommandToken& token);
    //void RubyTag(Parser::CommandToken& token);
    void StyleTag(Parser::CommandToken& token);
    //void UnlockLinkTag(Parser::CommandToken& token);

    /* 画像関連 */

    void ImageTag(const Parser::CommandToken& token) {
      Value<LayerPage> page;
      auto& args = token.arguments();
      auto layer = converter::ToLayerNum(args.find_or_throw(L"layer"));
      assert(layer.first == converter::LayerType::Foreground);
      Texture tex(args.find_or_throw(L"storage").ToStr());
      args.AttributeValTo(L"page", converter::ToPage, [&](LayerPage val) {
        page = val;
      });
      executor_.CommandImage(layer.second, page, tex);
    }
  };


  FileExecutor::Pimpl::Pimpl(const Executor& exe) :executor_(exe) {
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
  void FileExecutor::Pimpl::LTag(Parser::CommandToken &) {
    executor_.CommandL();
  }

  void FileExecutor::Pimpl::LocateTag(Parser::CommandToken & token) {
    Value<int> x, y;
    auto& args = token.arguments();
    using namespace converter;
    args.AttributeValTo(L"x", ToInt10, [&](int val) { x = val; });
    args.AttributeValTo(L"y", ToInt10, [&](int val) { y = val; });

    args.IfNotEmptyException();
    executor_.CommandLocate(x, y);
  }

  void FileExecutor::Pimpl::RTag(Parser::CommandToken &) {
    executor_.CommandR();
  }

  void FileExecutor::Pimpl::ResetFontTag(Parser::CommandToken &) {
    executor_.CommandResetFont();
  }

  void FileExecutor::Pimpl::ResetStyleTag(Parser::CommandToken &) {
    executor_.CommandResetStyle();
  }

  void FileExecutor::Pimpl::StyleTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandStyle([=](StyleCommandEditor& editor) mutable {
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

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::NoWaitTag(Parser::CommandToken &) {
    executor_.CommandNoWait();
  }

  void FileExecutor::Pimpl::PTag(Parser::CommandToken &) {
    executor_.CommandP();
  }

  void FileExecutor::Pimpl::DelayTag(Parser::CommandToken & token) {
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

    args.IfNotEmptyException();
  }

  void FileExecutor::Pimpl::EndIndentTag(Parser::CommandToken &) {
    executor_.CommandEndIndent();
  }

  void FileExecutor::Pimpl::EndNoWaitTag(Parser::CommandToken &) {
    executor_.CommandEndNoWait();
  }

  void FileExecutor::Pimpl::ERTag(Parser::CommandToken &) {
    executor_.CommandER();
  }

  void FileExecutor::Pimpl::CMTag(Parser::CommandToken &) {
    executor_.CommandCM();
  }

  void FileExecutor::Pimpl::CTTag(Parser::CommandToken &) {
    executor_.CommandCT();
  }

  void FileExecutor::Pimpl::CurrentTag(Parser::CommandToken & token) {
    Value<int> layer;
    LayerPage page = LayerPage::Fore;

    auto& args = token.arguments();
    using namespace converter;
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { layer = val; });
    args.AttributeValTo(L"page", ToPage, [&](LayerPage val) { page = val; });
    args.IfNotEmptyException();
    executor_.CommandCurrent(layer(), page);
  }

  void FileExecutor::Pimpl::DefFontTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefFont([args](FontCommandEditor& editor) mutable {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::DefStyleTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefStyle([args = std::move(args)](DefaultStyleCommandEditor& editor)mutable {
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

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::FontTTag(Parser::CommandToken & token) {

    auto& args = token.arguments();
    executor_.CommandFont([args = std::move(args)](FontCommandEditor& editor)mutable {
      using namespace converter;
      args.AttributeVal(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.AttributeValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.AttributeValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.AttributeValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

      args.AttributeValTo(L"color", ToColor, [&](const Color& val) {editor.color(val); });

      args.AttributeValTo(L"shadow", ToBool, [&](bool val) {editor.is_shadow(val); });

      args.AttributeValTo(L"shadowcolor", ToColor, [&](const Color& val) { editor.shadowcolor(val); });

      args.IfNotEmptyException();
    });

  }

  void FileExecutor::Pimpl::IndentTag(Parser::CommandToken &) {
    executor_.CommandIndent();
  }

  void FileExecutor::Pimpl::PositionTTag(Parser::CommandToken & token) {
    using namespace converter;
    Value<int> index;
    Value<LayerPage> page;
    auto& args = token.arguments();
    args.AttributeValTo(L"layer", ToMessageLayerNum, [&](int val) { index = val; });

    args.AttributeValTo(L"page", ToPage, [&](LayerPage val) { page = val; });

    executor_.CommandPosition(index, page, [args = std::move(args)](PositionCommandEditor& editor) mutable {
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

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::CHTag(Parser::CommandToken & token) {
    executor_.CommandTextNoDelay(token.arguments().find_or_throw(L"text"));
  }

}
