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

    void ImageTag(Parser::CommandToken& token);

    /* レイヤ関連 */
    void MoveTag(Parser::CommandToken& token);
    void TransTag(Parser::CommandToken& token);

  };

}
