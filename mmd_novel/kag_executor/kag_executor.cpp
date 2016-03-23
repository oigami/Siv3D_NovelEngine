#include "kag_executor.h"
#include <kag_executor/tag_editor.h>
namespace kag {
  Executor::Executor() {

    Clear();
  }
  void Executor::Clear() {
    message_manager_.Clear();
  }

  bool Executor::IsWait() const {
    if (message_manager_.IsWait())return true;
    return false;
  }

  void Executor::CommandL() {
    command_.push([&]() {
      message_manager_.SetWaitClick();
    });
  }

  void Executor::CommandR() {
    command_.push([&]() {
      message_manager_.AppendNewLine();
    });
  }

  void Executor::CommandP() {
    command_.push([&]() {
      message_manager_.SetWaitClick();
    });
  }

  void Executor::CommandDelay(int delay_time) {
    command_.push([&]() {
      message_manager_.SetDelayTime(delay_time);
    });
  }

  void Executor::CommandNoWait() {
    command_.push([&]() {
      message_manager_.SetNoWaitText(true);
    });
  }

  void Executor::CommandEndNoWait() {
    command_.push([&]() {
      message_manager_.SetNoWaitText(false);
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

  void Executor::CommandTextNoDelay(const SnapShotSpan & str) {
    command_.push([=]() {
      message_manager_.Current().Append(str.ToStr());
    });
  }

  void Executor::CommandCurrent(int index, int page) {
    command_.push([=]() {
      message_manager_.SetCurrent(index, page == 0 ? LayerPage::Fore : LayerPage::Back);
    });
  }

  void Executor::CommandIndent() {
    command_.push([this]() {
      this->message_manager_.Current().SetIndent();
    });
  }

  void Executor::CommandEndIndent() {
    command_.push([this]() {
      this->message_manager_.Current().SetEndIndent();
    });
  }

  void Executor::CommandLocate(Value<int> x, Value<int> y) {
    command_.push([=]() {
      if (x == kag::default) {
        message_manager_.Current().SetLocateY(y);
      } else if (y == kag::default) {
        message_manager_.Current().SetLocateX(x);
      } else {
        message_manager_.Current().SetLocate(x, y);
      }
    });
  }

  void Executor::CommandStyle(const CommandFunc<StyleCommandEditor>& f) {
    command_.push([f, this]() {
      StyleCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::CommandFont(const CommandFunc<FontCommandEditor>& f) {
    command_.push([f, this]() {
      FontCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::CommandDefFont(const CommandFunc<DefFontCommandEditor>& f) {
    command_.push([f, this]() {
      DefFontCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::CommandResetFont() {
    command_.push([=]() {
      message_manager_.Current().ResetFont();
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

  void Executor::ShowErrorMsg(const String & str) const {
    MessageBox::Show(str);
  }

  bool Executor::Update() {
    message_manager_.Update();
    return CommandUpdate();
  }

  bool Executor::CommandUpdate() {
    while (!command_.empty()) {
      if (IsWait()) return false;
      command_.front()();
      command_.pop();
    }
    return true;
  }

  void Executor::Draw() {
    message_manager_.Draw();
  }


}
