#include <kag_executor/message_layer.h>
namespace kag {

  MessageLayer::MessageLayer() {
    position_.set(Point(0, 0), Point(600, 400));
    Clear();
  }

  void MessageLayer::Clear() {
    limit_line_num = 0;
    NextPage();
  }

  void MessageLayer::NextPage() {
    text_line_.clear();
    limit_line_num = 0;
    now_height = 0;
    if (overflow_text) {
      text_line_.emplace_back(*overflow_text);
      CheckByReturn();
    } else {
      text_line_.emplace_back(0, now_font_);
    }
    limit_line_num = 1;
  }

  void MessageLayer::Append(const String & str) {
    text_line_.back().Append(str);
    CheckByReturn();
    return;
  }

  void MessageLayer::Append(const wchar & str) {
    text_line_.back().Append(str);

    //Print(str);
    CheckByReturn();
  }

  bool MessageLayer::IsLimitHeihgt() {
    return position_.h <= now_height + now_font_.Height();
  }

  void MessageLayer::AppenNewLine() {
    now_height += text_line_.back().Height();
    text_line_.emplace_back(0, now_font_);
    if (!IsLimitHeihgt()) {
      limit_line_num = text_line_.size();
    }
  }

  void MessageLayer::Draw() const {
    int y = 0;
    for (auto& i : step(limit_line_num)) {
      y = text_line_[i].Draw(position_.x, y);
    }

    position_.drawFrame();
  }

  inline void MessageLayer::CheckByReturn() {
    assert(!IsLimitHeihgt());
    auto opt = text_line_.back().ByReturn(position_.w);
    if (!opt)
      return;

    now_height += text_line_.back().Height();
    if (!IsLimitHeihgt()) {
      text_line_.emplace_back(*opt);
      limit_line_num = text_line_.size();
    } else {
      overflow_text = std::move(opt);
    }
  }

  namespace message {

    MessageTextLine::MessageTextLine(const int x, MessageTextFont & font) {
      Append({ x,font });
    }

    MessageTextLine::MessageTextLine(const MessageText & text) {
      Append(text);
    }

    void MessageTextLine::Clear() {
      const auto font = text_.back().Font();
      text_.clear();
      Append({ 0, font });
    }

    int MessageTextLine::Draw(int x, int y) const {
      const int new_y = y + max_height_;
      for (auto& i : text_) {
        x += i.Draw(x, new_y).w;
      }
      return new_y;
    }

    void MessageTextLine::Append(const String & str) {
      text_.back().Append(str);
    }

    void MessageTextLine::Append(const wchar & str) {
      text_.back().Append(str);
    }

    void MessageTextLine::Append(const MessageText & text) {
      max_height_ = std::max(max_height_, text.Height());
      text_.push_back(text);
    }

    Optional<MessageText> MessageTextLine::ByReturn(int width) {
      auto res = text_.back().ByReturn(width);
      return{ res };
    }

    void MessageTextLine::AppendNewFont(const MessageTextFont & font) {
      text_.emplace_back(text_.back().GetWidth(), font);
      max_height_ = std::max(max_height_, font.Height());
    }
  }
}
