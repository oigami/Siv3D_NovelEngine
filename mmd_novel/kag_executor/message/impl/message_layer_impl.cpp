#include <MmdNovel/message/message_layer_impl.h>
namespace kag {
  namespace message {

    TextLine::TextLine(int x, const Text & text)
      : max_height_(0) {
      Append(x, text);
    }

    TextLine::TextLine(const Text & text) : TextLine(0, text) {
    }

    void TextLine::Clear() {
      const auto font = text_.back().text_.Font();
      text_.clear();
      max_height_ = 0;
      Append(Text{ font });
    }

    int TextLine::Draw(int x, int y) const {
      const int new_y = y + Height();
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
      Append(text_.back().x, text);
    }

    void TextLine::Append(int x, const Text & text) {
      max_height_ = std::max(max_height_, text.Height());
      text_.push_back({ text,x });
    }

    Optional<Text> TextLine::ByReturn(int width) {
      // すでに使用している分を引いて使える部分の幅のみで計算する
      auto res = text_.back().text_.ByReturn(width - text_.back().x);
      return{ res };
    }

    void TextLine::AppendNewFont(const TextFont & font) {
      auto& pre_text = text_.back();
      int x = pre_text.x + pre_text.text_.GetWidth();
      AppendNewFont(x, font);
    }

    void TextLine::AppendNewFont(int x, const TextFont & font) {
      Append(x, font);
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



    Text::Text(const TextFont & font, String && text)
      : font_(font), text_(std::move(text)) {
    }

    Text::Text(const TextFont & font)
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
    const TextFont & Text::Font() const { return font_; }
  }
}
