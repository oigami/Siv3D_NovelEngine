#pragma once
#include <queue>
#include <kag_parser/kag_parser.h>
#include <kag_executor/message_layer.h>
#include <kag_executor/message_manager.h>
#include <kag_executor/tag_editor.h>
namespace kag {

  class Executor {
  public:

    Executor();

    /// <summary>
    /// 次のコマンドに進める時にtrueを返す
    /// </summary>
    /// <returns></returns>
    bool Update();
    bool CommandUpdate() {
      while (!is_wait_click_ && !command_.empty()) {
        if (message_manager_.IsFlush() == false) return false;
        command_.front()();
        command_.pop();
      }
      return true;
    }
    void Draw();
    void Clear();

    /* コマンド */

    void CommandL();

    void CommandR();

    void CommandP();

    void CommandDelay(int delay_time);

    void CommandER();

    void CommandCM();

    void CommandCT();

    /// <summary>
    /// テキストをメッセージレイヤに送る
    /// </summary>
    void CommandText(const SnapShotSpan& str);
    template<class Editor>
    using CommandFunc = std::function<void(Editor&)>;

    void CommandFont(const CommandFunc<FontCommandEditor>& f);

    void CommandPosition(Value<int> layer, Value<int> page, const CommandFunc<PositionCommandEditor>& f);

  protected:
    void ShowErrorMsg(const String& str) const {
      MessageBox::Show(str);
    }

  protected:

    std::queue<std::function<void()>> command_;
    MessageManager message_manager_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page;

    /// <summary>クリックによるスキップをするかどうか</summary>
    bool is_click_skip;
  };
}
