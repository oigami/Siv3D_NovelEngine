#pragma once
#include <kag_executor/kag_executor.h>
namespace kag {
  class FileExecutor : public Executor {
  public:
    FileExecutor();
    FileExecutor(const FilePath& filename);
    void Update();

  private:

    Parser parser_;

    using TagFunction = void(FileExecutor::*)(const kag::Parser::CommandToken&);
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
    //void DefFontTag(const Parser::CommandToken& token);
    //void DefStyleTag(const Parser::CommandToken& token);
    void DelayTag(const Parser::CommandToken& token);
    //void EndIndentTag(const Parser::CommandToken& token);
    //void EndNoWaitTag(const Parser::CommandToken& token);
    void ERTag(const Parser::CommandToken& token);
    void FontTTag(const Parser::CommandToken& token);
    //void GlyphTag(const Parser::CommandToken& token);
    //void GraphTag(const Parser::CommandToken& token);
    //void HCHTag(const Parser::CommandToken& token);
    //void IndentTag(const Parser::CommandToken& token);
    //void LTagTag(const Parser::CommandToken& token);
    void LTag(const Parser::CommandToken& token);
    //void LocateTag(const Parser::CommandToken& token);
    //void LockLinkTag(const Parser::CommandToken& token);
    //void NoWaitTag(const Parser::CommandToken& token);
    void PTag(const Parser::CommandToken& token);
    void PositionTTag(const Parser::CommandToken& token);
    void RTag(const Parser::CommandToken& token);
    //void ResetFontTag(const Parser::CommandToken& token);
    //void ResetStyleTag(const Parser::CommandToken& token);
    //void RubyTag(const Parser::CommandToken& token);
    //void StyleTag(const Parser::CommandToken& token);
    //void UnlockLinkTag(const Parser::CommandToken& token);

  };
}
