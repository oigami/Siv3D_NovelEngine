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
    void LTag(const Parser::CommandToken& token);
    void RTag(const Parser::CommandToken& token);
    void PTag(const Parser::CommandToken& token);
    void DelayTag(const Parser::CommandToken& token);
    void ERTag(const Parser::CommandToken& token);
    void CMTag(const Parser::CommandToken& token);
    void CTTag(const Parser::CommandToken& token);
    void FontTTag(const Parser::CommandToken& token);
    void PositionTTag(const Parser::CommandToken& token);
    void CHTag(const Parser::CommandToken& token);

  };
}
