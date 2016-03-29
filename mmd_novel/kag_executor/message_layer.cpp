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
      text_line_.emplace_back(0, message::Text(now_font_));
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
    text_line_.emplace_back(sum_height_, message::Text(now_font_));
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
    text_line_.emplace_back(y, message::TextLine::TextWithX(now_font_, x));
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

  void MessageLayer::SetDefaultStyle(const message::DefaultStyle & style) {
    default_style_ = style;
  }

  void MessageLayer::ResetStyle() {
    SetLineSpacing(default_style_.line_spacing_);
    SetLineSize(default_style_.line_size_);
  }

  void MessageLayer::CheckByReturn() {
    assert(!IsLimitHeihgt());
    int line_spacing = text_line_.back().LineSpacing();
    for (;;) {
      auto opt = text_line_.back().ByReturn(position_.w - margin_.x - margin_.w);
      if (!opt)
        return;

      message::TextLine::TextWithX text;
      if (indent_width_ != InvalidIndent) {
        text = { *opt, indent_width_ };
      } else {
        text = { *opt };
      }
      sum_height_ += text_line_.back().Height();
      if (!IsLimitHeihgt()) {
        text_line_.emplace_back(sum_height_, text);
        text_line_.back().SetLineSpacing(line_spacing);
        limit_line_num = static_cast<int>(text_line_.size());
      } else {
        overflow_text = std::move(opt);
        return;
      }
    }
  }

  namespace message {

    TextLine::TextLine(int y, const TextWithX & text)
      : max_height_(0), y_(y) {
      Append(text);
    }

    void TextLine::Clear() {
      const auto font = text_.back().text_.Font();
      text_.clear();
      max_height_ = 0;
      Append(Text{ font });
    }

    int TextLine::Draw(int x, int y) const {
      const int new_y = y_ + y + Height();
      for (auto& i : text_) {
        i.text_.Draw(x + i.x, new_y);
      }
      return new_y;
    }

    void TextLine::Append(const String & str) {
      text_.back().text_.Append(str);
    }

    void TextLine::Append(const wchar & str) {
      text_.back().text_.Append(str);
    }

    void TextLine::Append(const Text & text) {
      Append({ text,text_.back().x });
    }

    void TextLine::Append(const TextWithX & text) {
      max_height_ = std::max(max_height_, text.text_.Height());
      text_.push_back(text);
    }

    Optional<Text> TextLine::ByReturn(int width) {
      auto res = text_.back().text_.ByReturn(width - text_.back().x);
      return{ res };
    }

    void TextLine::AppendNewFont(const MessageTextFont & font) {
      auto& pre_text = text_.back();
      int x = pre_text.x + pre_text.text_.GetWidth();
      AppendNewFont(x, font);
    }

    void TextLine::AppendNewFont(int x, const MessageTextFont & font) {
      Append({ font, x });
    }

    int TextLine::Height() const {
      return style_.line_spacing_ + style_.LineSize(max_height_);
    }

    int TextLine::Width() const { return text_.back().text_.GetWidth(); }

    void TextLine::SetLineSize(int px) {
      style_.line_size_ = px;
    }

    void TextLine::ResetLineSize() {
      style_.ResetLineSize();
    }

    int TextLine::LineSpacing() const { return style_.line_spacing_; }

    void TextLine::SetLineSpacing(int px) {
      style_.line_spacing_ = px;
    }

    void TextLine::ResetLineSpacing() {
      style_.ResetLineSpacing();
    }

    Text::Text(const MessageTextFont & font, String && text)
      : font_(font), text_(std::move(text)) {
    }

    Text::Text(const MessageTextFont & font)
      : font_(font) {
    }

    RectF Text::Draw(int x, int y_add_fontheight) const {
      return font_.Draw(text_, x, y_add_fontheight);
    }

    Text & Text::Append(const String & s) {
      text_.append(s);
      return *this;
    }

    Text & Text::Append(const wchar & s) {
      text_ += s;
      return *this;
    }

    Optional<Text> Text::ByReturn(int width) {

      // すでに使用している分を引いて使える部分の幅のみで計算する
      size_t index = font_.drawableCharacters(text_, width);
      if (text_.length == index) return none;
      String s = text_.substr(index);
      text_.resize(index);
      return Text(font_, std::move(s));
    }
    int Text::GetWidth() const {
      return font_.region(text_).w;
    }
    int Text::Height() const {
      return font_.Height();
    }
    const MessageTextFont & Text::Font() const { return font_; }
  }
}
