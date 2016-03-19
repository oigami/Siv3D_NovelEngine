#include "kag_executor.h"
#include <kag_executor/tag_editor.h>
namespace kag {
  Executor::Executor() {

    Clear();
  }
  void Executor::Clear() {
    is_wait_click_ = false;
    is_click_new_page = false;
    message_manager_.Clear();
  }

  void Executor::CommandL() {
    is_wait_click_ = true;
  }

  void Executor::CommandR() {
    message_manager_.AppendNewLine();
  }

  void Executor::CommandP() {
    is_wait_click_ = true;
  }

  void Executor::CommandDelay(int delay_time) {
    message_manager_.SetDelayTime(delay_time);
  }

  void Executor::CommandER() {
    message_manager_.Clear();
  }

  void Executor::CommandCM() {
    message_manager_.AllClear();
  }

  void Executor::CommandCT() {
    CommandCM();
    message_manager_.SetCurrent(0, LayerPage::Fore);
  }

  void Executor::CommandText(const SnapShotSpan& str) {
    message_manager_.Append(str);
  }

  FontCommandEditor Executor::CommandFont() {
    return FontCommandEditor(message_manager_);
  }

  PositionCommandEditor Executor::CommandPosition(Value<int> layer, Value<int> page) {
    if (layer == kag::default) layer = message_manager_.CurrentLayerNum();
    if (page == kag::default) page = message_manager_.CurrentPageNum();
    return PositionCommandEditor(message_manager_.GetLayer(layer, page));
  }

  bool Executor::Update() {
    if (!message_manager_.Update()) {
      if (Input::MouseL.clicked)
        message_manager_.NextPage();
      return false;
    } else if (message_manager_.IsFlush() == false) {
      if (Input::MouseL.clicked)
        message_manager_.Flush();
      return false;
    }
    if (is_wait_click_) {
      if (!Input::MouseL.clicked)
        return false;
      is_wait_click_ = false;
      if (is_click_new_page) {
        is_click_new_page = false;
        message_manager_.NextPage();
      }
    }
    return true;
  }

  void Executor::Draw() {
    message_manager_.Draw();
  }


}
