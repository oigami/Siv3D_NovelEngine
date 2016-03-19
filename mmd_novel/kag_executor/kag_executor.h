#pragma once
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

    FontCommandEditor CommandFont();

    PositionCommandEditor CommandPosition(Value<int> layer, Value<int> page);

  protected:
    void ShowErrorMsg(const String& str) const {
      MessageBox::Show(str);
    }

  protected:

    MessageManager message_manager_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page;

  };
}
