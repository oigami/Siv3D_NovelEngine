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
    command_.push([&]() {
      is_wait_click_ = true;
    });
  }

  void Executor::CommandR() {
    command_.push([&]() {
      message_manager_.AppendNewLine();
    });
  }

  void Executor::CommandP() {
    command_.push([&]() {
      is_wait_click_ = true;
    });
  }

  void Executor::CommandDelay(int delay_time) {
    command_.push([&]() {
      message_manager_.SetDelayTime(delay_time);
    });
  }

  void Executor::CommandER() {
    command_.push([&]() {
      message_manager_.Clear();
    });
  }

  void Executor::CommandCM() {
    command_.push([&]() {
      message_manager_.AllClear();
    });
  }

  void Executor::CommandCT() {
    command_.push([&]() {
      CommandCM();
      message_manager_.SetCurrent(0, LayerPage::Fore);
    });
  }

  void Executor::CommandText(const SnapShotSpan& str) {
    command_.push([=]() {
      message_manager_.Append(str);
    });
  }

  void Executor::CommandFont(const CommandFunc<FontCommandEditor>& f) {
    command_.push([f, this]() {
      FontCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::CommandPosition(Value<int> layer, Value<int> page, const CommandFunc<PositionCommandEditor>& f) {
    command_.push([=]()mutable {
      if (layer == kag::default) layer = message_manager_.CurrentLayerNum();
      if (page == kag::default) page = message_manager_.CurrentPageNum();
      PositionCommandEditor editor(message_manager_.GetLayer(layer, page));
      f(editor);
      editor.Commit();
    });
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
    while (!is_wait_click_ && !command_.empty()) {
      command_.front()();
      command_.pop();
    }
    return true;
  }

  void Executor::Draw() {
    message_manager_.Draw();
  }


}
