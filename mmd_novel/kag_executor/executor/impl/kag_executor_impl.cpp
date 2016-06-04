#include "kag_executor_impl.h"
#pragma comment(lib,"d3d11.lib")
namespace kag {
  Executor::Pimpl::Pimpl() {
    layer_manager_ = std::make_shared<LayerManagerImpl>();
    message_manager_.SetLayerManager(layer_manager_);
    image_manager_->SetLayerManager(layer_manager_);
    Clear();
  }

  void Executor::Pimpl::Clear() {
    message_manager_.Clear();
  }

  bool Executor::Pimpl::IsWait() const {
    if (message_manager_.IsWait())return true;
    return false;
  }

  void Executor::Pimpl::Command(const std::function<void()>& f) {
    command_.push(f);
  }

  void Executor::Pimpl::CommandL() {
    command_.push([this]() {
      message_manager_.SetWaitClick();
    });
  }

  void Executor::Pimpl::CommandR() {
    command_.push([this]() {
      message_manager_.AppendNewLine();
    });
  }

  void Executor::Pimpl::CommandP() {
    command_.push([this]() {
      message_manager_.SetWaitClick();
    });
  }

  void Executor::Pimpl::CommandDelay(int delay_time) {
    command_.push([delay_time, this]() {
      message_manager_.SetDelayTime(delay_time);
    });
  }

  void Executor::Pimpl::CommandNoWait() {
    command_.push([this]() {
      message_manager_.SetNoWaitText(true);
    });
  }

  void Executor::Pimpl::CommandEndNoWait() {
    command_.push([this]() {
      message_manager_.SetNoWaitText(false);
    });
  }

  void Executor::Pimpl::CommandER() {
    command_.push([this]() {
      message_manager_.Clear();
    });
  }

  void Executor::Pimpl::CommandCM() {
    command_.push([this]() {
      message_manager_.AllClear();
    });
  }

  void Executor::Pimpl::CommandCT() {
    command_.push([this]() {
      CommandCM();
      message_manager_.SetCurrent(0, LayerPage::Fore);
    });
  }

  void Executor::Pimpl::CommandText(const SnapShotSpan & str) {
    command_.push([str, this]() {
      message_manager_.Append(str);
    });
  }

  void Executor::Pimpl::CommandTextNoDelay(const SnapShotSpan & str) {
    command_.push([str, this]() {
      message_manager_.Current()->Append(str.ToStr());
    });
  }

  void Executor::Pimpl::CommandCurrent(int index, const LayerPage& page) {
    command_.push([index, page, this]() {
      message_manager_.SetCurrent(index, page);
    });
  }

  void Executor::Pimpl::CommandIndent() {
    command_.push([this]() {
      this->message_manager_.Current()->BeginIndent();
    });
  }

  void Executor::Pimpl::CommandEndIndent() {
    command_.push([this]() {
      this->message_manager_.Current()->EndIndent();
    });
  }

  void Executor::Pimpl::CommandLocate(Value<int> x, Value<int> y) {
    command_.push([x, y, this]() {
      if (x == Define::default) {
        message_manager_.Current()->SetLocateY(y());
      } else if (y == Define::default) {
        message_manager_.Current()->SetLocateX(x());
      } else {
        message_manager_.Current()->SetLocate(x(), y());
      }
    });
  }

  void Executor::Pimpl::CommandStyle(const CommandFunc<StyleCommandEditor>& f) {
    command_.push([f, this]() {
      StyleCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::Pimpl::CommandDefStyle(const CommandFunc<DefaultStyleCommandEditor>& f) {
    command_.push([f, this]() {
      DefaultStyleCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::Pimpl::CommandResetStyle() {
    command_.push([this]() {
      message_manager_.Current()->ResetFont();
    });
  }

  void Executor::Pimpl::CommandFont(const CommandFunc<FontCommandEditor>& f) {
    command_.push([f, this]() {
      FontCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::Pimpl::CommandDefFont(const CommandFunc<DefFontCommandEditor>& f) {
    command_.push([f, this]() {
      DefFontCommandEditor editor(message_manager_);
      f(editor);
      editor.Commit();
    });
  }

  void Executor::Pimpl::CommandResetFont() {
    command_.push([this]() {
      message_manager_.Current()->ResetFont();
    });
  }

  void Executor::Pimpl::CommandPosition(Value<int> layer, const Value<LayerPage>& page, const CommandFunc<PositionCommandEditor>& f) {
    command_.push([layer, page, f, this]() {
      const int new_layer = (layer == Define::default ? message_manager_.CurrentLayerNum() : layer());
      const LayerPage l_page = (page == Define::default ? message_manager_.CurrentPage() : page());
      PositionCommandEditor editor(message_manager_.GetLayer(new_layer, l_page));
      f(editor);
      editor.Commit();
    });
  }

  void Executor::Pimpl::CommandImage(int layer, const Value<LayerPage>& page, const Texture & tex) {
    command_.push([tex, layer, page, this] {
      image_manager_->GetLayer(layer, page(LayerPage::Fore))->SetTex(tex);
    });
  }

  bool Executor::Pimpl::Update() {
    layer_manager_->Update();
    message_manager_.Update();
    return CommandUpdate();
  }

  bool Executor::Pimpl::CommandUpdate() {
    while (!command_.empty()) {
      if (IsWait()) return false;
      command_.front()();
      command_.pop();
    }
    return true;
  }

  void Executor::Pimpl::Draw() {
    layer_manager_->Draw();
  }
}
