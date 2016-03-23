#include <kag_executor/message_layer.h>
namespace kag {

  MessageLayer::MessageLayer() {
    position_.set(Point(16, 16), Point(Window::Size().x - 32, Window::Size().y - 32));
    margin_.set({ 8,8 }, { 8,8 });
    is_visible_ = false;
    background_color_ = Palette::Gray;
    background_color_.a = 128;
    indent_width_ = InvalidIndent;
    Clear();
  }

  void MessageLayer::Clear() {
    limit_line_num = 0;
    NextPage();
  }

  void MessageLayer::NextPage() {
    text_line_.clear();
    limit_line_num = 0;
    sum_height_ = 0;
    if (overflow_text) {
      text_line_.emplace_back(0, *overflow_text);
      overflow_text = none;
      CheckByReturn();
    } else {
      text_line_.emplace_back(0, message::MessageText(0, now_font_));
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
    return position_.h - margin_.h - margin_.y <= sum_height_ + text_line_.back().Height();
  }

  void MessageLayer::AppenNewLine() {
    sum_height_ += text_line_.back().Height();
    text_line_.emplace_back(sum_height_, message::MessageText(0, now_font_));
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
      text_line_[i].Draw(position_.x + margin_.x, y);
    }
  }

  const message::MessageTextFont & MessageLayer::NowFont() const { return now_font_; }

  void MessageLayer::SetFont(const message::MessageTextFont & font) {
    text_line_.back().AppendNewFont(font);
    now_font_ = font;
  }

  void MessageLayer::SetDefaultFont(const message::MessageTextFont & font) {
    default_font_ = font;
  }

  void MessageLayer::ResetFont() {
    SetFont(default_font_);
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

  void MessageLayer::SetLocate(int x, int y) {
    text_line_.emplace_back(y, message::MessageText(x, now_font_));
    sum_height_ = y;
  }

  void MessageLayer::SetLocateX(int x) {
    text_line_.back().AppendNewFont(x, now_font_);
  }

  void MessageLayer::SetLocateY(int y) {
    SetLocate(0, y);
  }

  void MessageLayer::SetIndent() {
    indent_width_ = text_line_.back().Width();
  }

  void MessageLayer::SetEndIndent() {
    indent_width_ = InvalidIndent;
  }

  void MessageLayer::SetLineSize(int px) {
    text_line_.back().SetLineSize(px);
  }

  void MessageLayer::ResetLineSize() {
    text_line_.back().ResetLineSize();
  }

  void MessageLayer::SetLineSpacing(int px) {
    text_line_.back().SetLineSpacing(px);
  }

  void MessageLayer::ResetLineSpacing() {
    text_line_.back().ResetLineSpacing();
  }

  void MessageLayer::CheckByReturn() {
    assert(!IsLimitHeihgt());
    int line_spacing = text_line_.back().LineSpacing();
    for (;;) {
      auto opt = text_line_.back().ByReturn(position_.w - margin_.x - margin_.w);
      if (!opt)
        return;
      
      if (indent_width_ != InvalidIndent)
        opt->Indent(indent_width_);
      sum_height_ += text_line_.back().Height();
      if (!IsLimitHeihgt()) {
        text_line_.emplace_back(sum_height_, *opt);
        text_line_.back().SetLineSpacing(line_spacing);
        limit_line_num = static_cast<int>(text_line_.size());
      } else {
        overflow_text = std::move(opt);
        return;
      }
    }
  }

  namespace message {

    MessageTextLine::MessageTextLine(int y, const MessageText & text)
      : max_height_(0), line_spacing_(0), line_size_(default_line_size), y_(y) {
      Append(text);
    }

    void MessageTextLine::Clear() {
      const auto font = text_.back().Font();
      text_.clear();
      max_height_ = 0;
      Append({ 0, font });
    }

    int MessageTextLine::Draw(int x, int y) const {
      const int new_y = y_ + y + Height();
      for (auto& i : text_) {
        i.Draw(x, new_y);
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

    inline void MessageTextLine::AppendNewFont(int x, const MessageTextFont & font) {
      text_.emplace_back(x, font);
      max_height_ = std::max(max_height_, font.Height());
    }

    int MessageTextLine::Height() const {
      return line_spacing_ + (line_size_ == default_line_size ? max_height_ : line_size_);
    }

    int MessageTextLine::Width() const { return text_.back().GetWidth(); }

    void MessageTextLine::SetLineSize(int px) {
      line_size_ = px;
    }

    void MessageTextLine::ResetLineSize() {
      line_size_ = default_line_size;
    }

    int MessageTextLine::LineSpacing() const { return line_spacing_; }

    void MessageTextLine::SetLineSpacing(int px) {
      line_spacing_ = px;
    }

    void MessageTextLine::ResetLineSpacing() {
      line_spacing_ = default_line_spacing;
    }

    MessageText::MessageText(int x, const MessageTextFont & font, String && text)
      : start_x_(x), font_(font), text_(std::move(text)) {
    }

    MessageText::MessageText(int x, const MessageTextFont & font)
      : start_x_(x), font_(font) {
    }

    void MessageText::Indent(int x) { start_x_ = x; }

    RectF MessageText::Draw(int x, int y_add_fontheight) const {
      return font_.Draw(text_, x + start_x_, y_add_fontheight);
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
