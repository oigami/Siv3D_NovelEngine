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
      while (!command_.empty()) {
        if (IsWait()) return false;
        command_.front()();
        command_.pop();
      }
      return true;
    }
    void Draw();
    void Clear();

    bool IsWait() const;

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

    void CommandTextNoDelay(const SnapShotSpan& str);

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

  };
}
