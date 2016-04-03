#pragma once
#include <MmdNovel/message/message_manager.h>
namespace kag {
  class MessageManager::Pimpl {
  public:
    Pimpl() {
      click_key_ = Input::MouseL | Input::KeyEnter;
      is_active_key = true;
      resize(2);
      message_layer_[0][0].SetVisible(true);
      delay_time_ = 30;
      delay_index_ = 0;
      current_page_ = 0;
      current_layer_ = 0;
    }

    void resize(size_t size) {
      message_layer_.resize(size);
    }

    void Clear() {
      is_wait_click_ = false;
      is_click_new_page = false;
      message_layer_[current_layer_][current_page_].Clear();
    }

    void AllClear() {
      for (auto& i : message_layer_) {
        for (auto& j : i) {
          j.Clear();
        }
      }
    }

    bool IsFlush() const {
      return delay_text.Length() == delay_index_;
    }

    void Flush() {
      auto& current = Current();
      for (int i = 0, len = delay_text.Length() - delay_index_; i < len; i++) {
        current.Append(delay_text[delay_index_++]);
        if (current.IsLimitHeihgt()) {
          is_click_new_page = true;
          is_wait_click_ = true;
          return;
        }
      }
    }

    void Append(const SnapShotSpan & text) {
      assert(IsFlush());
      if (is_no_wait_) {
        auto& current = Current();
        current.Append(text.ToStr());
        if (current.IsLimitHeihgt()) {
          is_click_new_page = true;
          is_wait_click_ = true;
          return;
        }
      } else {
        delay_text = text;
        timer_.restart();
        delay_index_ = 0;
      }
    }

    void AppendNewLine() {
      Current().AppenNewLine();
    }

    void NextPage() {
      Current().NextPage();
      timer_.set(MillisecondsF(delay_time_ * delay_index_));
    }

    void SetWaitClick() { is_wait_click_ = true; }

    void SetClickNextPage() { is_click_new_page = true; }

    bool Update() {
      if (is_wait_click_) {
        if (CheckClicked()) {
          if (is_click_new_page)
            NextPage();
          is_click_new_page = false;
          is_wait_click_ = false;
        }
        return false;
      }
      auto& current = Current();
      int ms = timer_.ms();
      int loop = ms / delay_time_ - delay_index_;
      loop = std::min(loop, delay_text.Length() - delay_index_);
      for (int i = 0; i < loop; i++) {
        current.Append(delay_text[delay_index_++]);
        if (current.IsLimitHeihgt()) {
          is_click_new_page = true;
          is_wait_click_ = true;
          return false;
        }
      }

      if (IsFlush() == false) {
        if (CheckClicked())
          Flush();
        return false;
      }

      return true;
    }

    void Draw() const {
      for (auto& i : message_layer_) {
        i[message_fore_layer].Draw();
      }
    }

    void SetCurrentLayer(int layer_index) {
      current_layer_ = layer_index;
    }

    void SetCurrent(int layer_index, LayerPage type) {
      current_layer_ = layer_index;
      current_page_ = type == LayerPage::Fore;
    }

    void SetDelayTime(int delay_time) {
      delay_time_ = delay_time;
    }

    void SetNoWaitText(bool is_no_wait) {
      is_no_wait_ = is_no_wait;
    }

    MessageLayer & GetLayer(int index, int page) {
      return message_layer_[index][page];
    }

    MessageLayer & Current() { return GetLayer(current_layer_, current_page_); }

    int CurrentLayerNum() const { return current_layer_; }

    int CurrentPageNum() const { return current_page_; }

    void SetClickKey(const KeyCombination & key) {
      click_key_ = key;
    }

    void SetInvalidKeyInput() { is_active_key = false; }

    void SetValidKeyInput() { is_active_key = true; }

    bool IsWait() const { return is_wait_click_ || !IsFlush(); }

    bool CheckClicked() const {
      if (is_active_key) return click_key_.clicked;
      return false;
    }

  private:

    Stopwatch timer_;

    /// <summary>
    /// 表が0、裏が1の配列の配列
    /// </summary>
    Array<std::array<MessageLayer, 2>> message_layer_;
    static constexpr int message_fore_layer = 0;
    static constexpr int message_back_layer = 1;

    /// <summary>
    /// 現在のレイヤ番号を表す
    /// </summary>
    int current_layer_;

    /// <summary>
    /// 現在のページを表す 表が0、裏が1
    /// </summary>
    int current_page_;

    /// <summary>遅延表示してるテキスト</summary>
    SnapShotSpan delay_text;

    /// <summary>次に表示する遅延テキストの添字</summary>
    int delay_index_;

    /// <summary>一文字あたりに遅延する時間</summary>
    int delay_time_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page;

    /// <summary>クリックによるスキップをするかどうか</summary>
    bool is_click_skip;

    bool is_no_wait_;

    /// <summary>クリックに対応するキー</summary>
    KeyCombination click_key_;

    /// <summary>キー入力が有効かどうか</summary>
    bool is_active_key;
  };

}
