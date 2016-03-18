#pragma once
#include <kag_parser/kag_parser.h>
#include <kag_executor/message_layer.h>
#include <kag_executor/message_manager.h>
namespace kag {

  class Executor {
  public:

    Executor(const FilePath& path);
    Executor() {}

    void Update();
    void Draw();
    void Clear();

  private:
    Parser parser_;

    MessageManager message_manager_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page;

    using TagFunction = void(Executor::*)(const kag::Parser::CommandToken&);
    std::map<SnapShotSpan, TagFunction> tag_func_;

    void ShowErrorMsg(const String& str) const {
      MessageBox::Show(str);
    }
  private:

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
  };
}
