#include <kag_executor/message_manager.h>
namespace kag {
  MessageManager::MessageManager() {
    resize(2);
    delay_time_ = 30;
    delay_index_ = 0;
    current_page_ = 0;
    current_layer_ = 0;
  }

  void MessageManager::resize(size_t size) {
    message_layer_.resize(size);
  }

  void MessageManager::Clear() {
    message_layer_[current_layer_][current_page_].Clear();
  }

  void MessageManager::AllClear() {
    for (auto& i : message_layer_) {
      for (auto& j : i) {
        j.Clear();
      }
    }
  }

  bool MessageManager::IsFlush() const {
    return delay_text.Length() == delay_index_;
  }

  void MessageManager::Flush() {
    auto& current = Current();
    for (int i = 0, len = delay_text.Length() - delay_index_; i < len; i++) {
      current.Append(delay_text[delay_index_++]);
      if (current.IsLimitHeihgt()) return;
    }
  }

  void MessageManager::Append(const SnapShotSpan & text) {
    assert(IsFlush());
    delay_text = text;
    timer_.restart();
    delay_index_ = 0;
  }

  void MessageManager::AppendNewLine() {
    Current().AppenNewLine();
  }

  void MessageManager::NextPage() {
    Current().NextPage();
    timer_.set(MillisecondsF(delay_time_ * delay_index_));
  }

  bool MessageManager::Update() {
    auto& current = Current();
    if (current.IsLimitHeihgt()) return false;
    int ms = timer_.ms();
    int loop = ms / delay_time_ - delay_index_;
    loop = std::min(loop, delay_text.Length() - delay_index_);
    for (int i = 0; i < loop; i++) {
      current.Append(delay_text[delay_index_++]);
      if (current.IsLimitHeihgt()) return false;
    }
    return true;
  }

  void MessageManager::Draw() const {
    for (auto& i : message_layer_) {
      i[message_fore_layer].Draw();
    }
  }

  void MessageManager::SetCurrentLayer(int layer_index) {
    current_layer_ = layer_index;
  }

  void MessageManager::SetCurrent(int layer_index, LayerPage type) {
    current_layer_ = layer_index;
    current_page_ = type == LayerPage::Fore;
  }

  void MessageManager::SetDelayTime(int delay_time) {
    delay_time_ = delay_time;
  }

}
