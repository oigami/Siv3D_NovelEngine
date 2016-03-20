#include <kag_executor/message_layer.h>
namespace kag {

  MessageLayer::MessageLayer() {
    position_.set(Point(16, 16), Point(Window::Size().x - 32, Window::Size().y - 32));
    margin_.set({ 8,8 }, { 8,8 });
    is_visible_ = false;
    background_color_ = Palette::Gray;
    background_color_.a = 128;
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
      overflow_text = none;
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
    return position_.h - margin_.h - margin_.y <= now_height + now_font_.Height();
  }

  void MessageLayer::AppenNewLine() {
    now_height += text_line_.back().Height();
    text_line_.emplace_back(0, now_font_);
    if (!IsLimitHeihgt()) {
      limit_line_num = static_cast<int>(text_line_.size());
    }
  }

  void MessageLayer::Draw() const {
    if (!is_visible_) return;
    position_.draw(background_color_);
    if (!background_tex_.isEmpty())
      position_(background_tex_).draw();

    int y = position_.y + margin_.y;
    for (auto& i : step(limit_line_num)) {
      y = text_line_[i].Draw(position_.x + margin_.x, y);
    }
  }

  void MessageLayer::SetFont(const message::MessageTextFont & font) {
    text_line_.back().AppendNewFont(font);
    now_font_ = font;
  }

  void MessageLayer::SetPositionTop(int top) {
    position_.y = top;
  }

  void MessageLayer::SetPositionLeft(int left) {
    position_.x = left;
  }

  void MessageLayer::SetPositionWidth(int width) {
    position_.w = width;
  }

  void MessageLayer::SetPositionHeight(int height) {
    position_.h = height;
  }

  void MessageLayer::SetMarginTop(int top) {
    margin_.y = top;
  }

  void MessageLayer::SetMarginLeft(int left) {
    margin_.x = left;
  }

  void MessageLayer::SetMarginRight(int width) {
    margin_.w = width;
  }

  void MessageLayer::SetMarginBottom(int height) {
    margin_.h = height;
  }

  void MessageLayer::SetVisible(bool is_visible) {
    is_visible_ = is_visible;
  }

  void MessageLayer::SetBackgroundColor(Color argb) {
    background_color_ = argb;
  }

  void MessageLayer::SetBackgroundRGB(int r, int g, int b) {
    const int a = background_color_.a;
    background_color_ = Color(r, g, b, a);
  }

  void MessageLayer::SetBackgroundOpacity(int a) {
    background_color_ = a;
  }

  void MessageLayer::SetBackgroundTex(Texture tex) {
    background_tex_ = tex;
  }

  void MessageLayer::CheckByReturn() {
    assert(!IsLimitHeihgt());
    auto opt = text_line_.back().ByReturn(position_.w - margin_.x - margin_.w);
    if (!opt)
      return;

    now_height += text_line_.back().Height();
    if (!IsLimitHeihgt()) {
      text_line_.emplace_back(*opt);
      limit_line_num = static_cast<int>(text_line_.size());
    } else {
      overflow_text = std::move(opt);
    }
  }

  namespace message {

    MessageTextLine::MessageTextLine(const int x, MessageTextFont & font) :max_height_(0) {
      max_height_ = 0;
      Append({ x,font });
    }

    MessageTextLine::MessageTextLine(const MessageText & text) : max_height_(0) {
      Append(text);
    }

    void MessageTextLine::Clear() {
      const auto font = text_.back().Font();
      text_.clear();
      max_height_ = 0;
      Append({ 0, font });
    }

    int MessageTextLine::Draw(int x, int y) const {
      const int new_y = y + max_height_;
      for (auto& i : text_) {
        x += static_cast<int>(i.Draw(x, new_y).w);
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

    int MessageTextLine::Height() const {
      return max_height_;
    }

    MessageText::MessageText(int x, const MessageTextFont & font, String && text)
      : start_x_(x), font_(font), text_(std::move(text)) {
    }

    MessageText::MessageText(int x, const MessageTextFont & font)
      : start_x_(x), font_(font) {
    }

    RectF MessageText::Draw(int x, int y_add_fontheight) const {
      return font_.Draw(text_, x, y_add_fontheight);
    }

    MessageText & MessageText::Append(const String & s) {
      text_.append(s);
      return *this;
    }

    MessageText & MessageText::Append(const wchar & s) {
      text_ += s;
      return *this;
    }

    Optional<MessageText> MessageText::ByReturn(int width) {

      // すでに使用している分を引いて使える部分の幅のみで計算する
      size_t index = font_.drawableCharacters(text_, width - start_x_);
      if (text_.length == index) return none;
      String s = text_.substr(index);
      text_.resize(index);
      return MessageText(0, font_, std::move(s));
    }
    int MessageText::GetWidth() const {
      return start_x_ + font_.region(text_).w;
    }
    int MessageText::Height() const {
      return font_.Height();
    }
    const MessageTextFont & MessageText::Font() const { return font_; }
  }
}
