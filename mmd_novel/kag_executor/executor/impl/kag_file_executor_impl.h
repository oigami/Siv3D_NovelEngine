#pragma once
#include <MmdNovel/kag_file_executor.h>
#include <kag_parser/kag_parser.h>
namespace kag
{
  class FileExecutor::Pimpl
  {
  public:
    Pimpl(const Executor& exe);
    void Load(const FilePath& path);
    void Update();
    int NowFileLine() const;

    using TagFunction = IManager::FuncList;
    TagFunction tag_func_;
  private:
    Executor executor_;

    Parser parser_;

    //タグリファレンス
    //http://devdoc.kikyou.info/tvp/docs/kag3doc/contents/index.html

    //メッセージ関連

    //void CancelAutoModeTag(CommandToken& token);
    //void CancelSkipTag(CommandToken& token);
    void CHTag(CommandToken& token);
    void CMTag(CommandToken& token);
    void CTTag(CommandToken& token);
    void CurrentTag(CommandToken& token);
    void DefFontTag(CommandToken& token);
    void DefStyleTag(CommandToken& token);
    void DelayTag(CommandToken& token);
    void EndIndentTag(CommandToken& token);
    void EndNoWaitTag(CommandToken& token);
    void ERTag(CommandToken& token);
    void FontTTag(CommandToken& token);

    //void GlyphTag(CommandToken& token);
    //void GraphTag(CommandToken& token);
    //void HCHTag(CommandToken& token);
    void IndentTag(CommandToken& token);
    void LTag(CommandToken& token);
    void LocateTag(CommandToken& token);

    //void LockLinkTag(CommandToken& token);
    void NoWaitTag(CommandToken& token);
    void PTag(CommandToken& token);
    void PositionTTag(CommandToken& token);
    void RTag(CommandToken& token);
    void ResetFontTag(CommandToken& token);
    void ResetStyleTag(CommandToken& token);

    //void RubyTag(CommandToken& token);
    void StyleTag(CommandToken& token);

    //void UnlockLinkTag(CommandToken& token);

    /* 画像関連 */

    void ImageTag(CommandToken& token);

    /* レイヤ関連 */
    void MoveTag(CommandToken& token);
    void TransTag(CommandToken& token);

    /* MMD関連 */
    void CameraTag(CommandToken& token);

  };

}
