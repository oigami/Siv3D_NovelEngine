#pragma once
#include <MmdNovel/message/message_layer.h>
namespace kag {
  namespace message {

    /// <summary>
    /// フォント付き修飾テキスト
    /// </summary>
    class Text {
    public:
      Text() = default;

      Text(const TextFont& font, String&& text);

      Text(const TextFont& font);

      /// <summary>
      /// 文字列を描画する
      /// </summary>
      /// <param name="x">表示するx座標</param>
      /// <param name="y_add_fontheight">表示するy座標にfont.heightを足した座標</param>
      /// <returns></returns>
      RectF Draw(int x, int y_add_fontheight) const;

      /// <summary>
      /// 文字列を末尾に追加する
      /// </summary>
      /// <param name="s">文字列</param>
      /// <returns></returns>
      Text& Append(const String& s);

      /// <summary>
      /// 文字を末尾に追加する
      /// </summary>
      /// <param name="s">文字</param>
      /// <returns></returns>
      Text& Append(const wchar& s);

      /// <summary>
      /// 折り返し他部分を新しく生成して返す
      /// </summary>
      /// <param name="width">折り返す時の幅</param>
      /// <returns></returns>
      Optional<Text> ByReturn(int width);

      /// <summary>
      /// 指定したインデントx+文字列の幅を返す
      /// </summary>
      /// <returns></returns>
      int GetWidth() const;

      /// <summary>
      /// フォントの高さを返す
      /// </summary>
      /// <returns></returns>
      int Height() const;

      /// <summary>
      /// フォントを返す
      /// </summary>
      /// <returns></returns>
      const TextFont& Font() const;

    private:

      TextFont font_;
      String text_;
    };

    class TextLine {
    private:
      struct TextWithX {
        TextWithX() {}
        TextWithX(const Text& text, int x = 0) :text_(text), x(x) {}
        int x;
        Text text_;
      };
    public:

      /// <summary>
      /// コンストラクタ
      /// </summary>
      /// <param name="y">n行目のy座標</param>
      /// <param name="x">開始するx座標</param>
      /// <param name="text"></param>
      TextLine(int x, const Text& text);

      /// <summary>
      /// コンストラクタ
      /// </summary>
      /// <param name="y">n行目のy座標</param>
      /// <param name="text"></param>
      TextLine(const Text& text);


      void Clear();

      /// <summary>
      /// メッセージレイヤを基準として一行を描画する
      /// </summary>
      /// <param name="x">メッセージレイヤの左上（パディング等含）のx座標</param>
      /// <param name="y">メッセージレイヤの左上（パディング等含）のy座標</param>
      /// <returns></returns>
      int Draw(int x, int y) const;

      /// <summary>
      /// 文字列を末尾に追加する
      /// </summary>
      /// <param name="str"></param>
      void Append(const String& str);

      /// <summary>
      /// 文字を末尾に追加する
      /// </summary>
      /// <param name="str"></param>
      void Append(const wchar& str);

      /// <summary>
      /// 文字列をフォント付きで末尾に追加する
      /// </summary>
      /// <param name="text"></param>
      void Append(const Text& text);

      void Append(int x, const Text& text);

      /// <summary>
      /// 折り返し部分を新たに生成して返す
      /// </summary>
      /// <param name="width">折り返す幅</param>
      /// <returns></returns>
      Optional<Text> ByReturn(int width);

      /// <summary>
      /// フォントを末尾に追加する
      /// <para>ここで指定したフォントがAppend(wchar),Append(String)で使用される</para>
      /// </summary>
      /// <param name="font"></param>
      void AppendNewFont(const TextFont& font);

      void AppendNewFont(int x, const TextFont & font);

      /// <summary>
      /// 最大の高さを返す
      /// </summary>
      /// <returns></returns>
      int Height() const;

      /// <summary>
      /// 現在の文字列の横幅を返す
      /// </summary>
      /// <returns></returns>
      int Width() const;

      /// <summary>
      /// 行間を設定する
      /// </summary>
      /// <param name="px"></param>
      void SetLineSize(int px);

      /// <summary>
      /// 行間をリセットする
      /// </summary>
      void ResetLineSize();

      /// <summary>
      /// 一行全体の文字の縦幅を返す
      /// </summary>
      /// <returns></returns>
      int LineSpacing() const;

      /// <summary>
      /// 一行全体の文字の縦幅を返す
      /// </summary>
      /// <param name="px"></param>
      void SetLineSpacing(int px);

      /// <summary>
      /// 一行全体の文字の縦幅をリセットする
      /// </summary>
      void ResetLineSpacing();

    private:

      /// <summary>一行の中で最大の高さを保持する</summary>
      int max_height_;

      Style style_;

      Array<TextWithX> text_;

    };

  }

  class MessageLayer::Pimpl {
  public:

    Pimpl() {
      position_.set(Point(16, 16), Point(Window::Size().x - 32, Window::Size().y - 32));
      margin_.set({ 8,8 }, { 8,8 });
      is_visible_ = false;
      background_color_ = Palette::Gray;
      background_color_.a = 128;
      indent_width_ = InvalidIndent;
      Clear();
    }

    void Clear() {
      limit_line_num = 0;
      NextPage();
    }

    void NextPage() {
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

    void Append(const String & str) {
      text_line_.back().Append(str);
      CheckByReturn();
    }

    void Append(const wchar & str) {
      text_line_.back().Append(str);

      //Print(str);
      CheckByReturn();
    }

    bool IsLimitHeihgt() {
      return position_.h - margin_.h - margin_.y <= sum_height_ + text_line_.back().Height();
    }

    void AppenNewLine() {
      sum_height_ += text_line_.back().Height();
      text_line_.emplace_back(sum_height_, message::Text(now_font_));
      if (!IsLimitHeihgt()) {
        limit_line_num = static_cast<int>(text_line_.size());
      }
    }

    void Draw() const {
      if (!is_visible_) return;
      position_.draw(background_color_);
      if (!background_tex_.isEmpty())
        position_(background_tex_).draw();

      int y = position_.y + margin_.y;
      for (auto& i : step(limit_line_num)) {
        text_line_[i].Draw(position_.x + margin_.x, y + text_line_[i].y_);
      }
    }

    const message::TextFont & NowFont() const { return now_font_; }

    void SetFont(const message::TextFont & font) {
      text_line_.back().AppendNewFont(font);
      now_font_ = font;
    }

    void SetDefaultFont(const message::TextFont & font) {
      default_font_ = font;
    }

    void ResetFont() {
      SetFont(default_font_);
    }

    void SetPositionTop(int top) {
      position_.y = top;
    }

    void SetPositionLeft(int left) {
      position_.x = left;
    }

    void SetPositionWidth(int width) {
      position_.w = width;
    }

    void SetPositionHeight(int height) {
      position_.h = height;
    }

    void SetMarginTop(int top) {
      margin_.y = top;
    }

    void SetMarginLeft(int left) {
      margin_.x = left;
    }

    void SetMarginRight(int width) {
      margin_.w = width;
    }

    void SetMarginBottom(int height) {
      margin_.h = height;
    }

    void SetVisible(bool is_visible) {
      is_visible_ = is_visible;
    }

    void SetBackgroundColor(Color argb) {
      background_color_ = argb;
    }

    void SetBackgroundRGB(int r, int g, int b) {
      const int a = background_color_.a;
      background_color_ = Color(r, g, b, a);
    }

    void SetBackgroundOpacity(int a) {
      background_color_.a = a;
    }

    void SetBackgroundTex(Texture tex) {
      background_tex_ = tex;
    }

    void SetLocate(int x, int y) {
      text_line_.emplace_back(y, message::TextLine(x, now_font_));
      sum_height_ = y;
    }

    void SetLocateX(int x) {
      text_line_.back().AppendNewFont(x, now_font_);
    }

    void SetLocateY(int y) {
      SetLocate(0, y);
    }

    void BeginIndent() {
      indent_width_ = text_line_.back().Width();
    }

    void EndIndent() {
      indent_width_ = InvalidIndent;
    }

    void SetLineSize(int px) {
      text_line_.back().SetLineSize(px);
    }

    void ResetLineSize() {
      text_line_.back().ResetLineSize();
    }

    void SetLineSpacing(int px) {
      text_line_.back().SetLineSpacing(px);
    }

    void ResetLineSpacing() {
      text_line_.back().ResetLineSpacing();
    }

    void SetDefaultStyle(const message::DefaultStyle & style) {
      default_style_ = style;
    }

    void ResetStyle() {
      SetLineSpacing(default_style_.line_spacing_);
      SetLineSize(default_style_.line_size_);
    }
  private:

    /// <summary>
    /// 折り返しのチェックをする
    /// <para>もし折り返して高さに収まらなくなった場合はoverflow_textに入れる</para>
    /// </summary>
    void CheckByReturn() {
      assert(!IsLimitHeihgt());
      int line_spacing = text_line_.back().LineSpacing();
      for (;;) {
        auto opt = text_line_.back().ByReturn(position_.w - margin_.x - margin_.w);
        if (!opt)
          return;

        int indent = 0;
        if (indent_width_ != InvalidIndent)
          indent = indent_width_;
        sum_height_ += text_line_.back().Height();
        if (!IsLimitHeihgt()) {
          text_line_.emplace_back(sum_height_, message::TextLine(indent, *opt));
          text_line_.back().SetLineSpacing(line_spacing);
          limit_line_num = static_cast<int>(text_line_.size());
        } else {
          overflow_text = std::move(opt);
          return;
        }
      }
    }

    /// <summary>インデントが無効な時の値</summary>
    static constexpr int InvalidIndent = std::numeric_limits<int>::max();

    /// <summary>インデントの開始位置(InvalidIndentで無効)</summary>
    int indent_width_;

    /// <summary>現在設定されているフォント</summary>
    message::TextFont now_font_;

    /// <summary>デフォルトに設定されているフォント</summary>
    message::TextFont default_font_;

    /// <summary>デフォルトに設定されているフォント</summary>
    message::DefaultStyle default_style_;

    /// <summary>メッセージレイヤのサイズ</summary>
    Rect position_;

    /// <summary>
    /// メッセージを表示する部分のマージン
    /// <para>rect型だがposition_からの相対位置(内側が正)になっている</para>
    /// </summary>
    Rect margin_;

    /// <summary>背景テクスチャ</summary>
    Texture background_tex_;

    /// <summary>背景色</summary>
    Color background_color_;

    // TODO: 全体に反映させるための処理を追加する
    /// <summary>レイヤ全体の透明度</summary>
    int opacity_;

    /// <summary>表示するかどうか</summary>
    bool is_visible_;

    /// <summary>描画してる文字列の高さの合計</summary>
    int sum_height_;

    /// <summary>表示できた限界の行数</summary>
    int limit_line_num;

    struct TextLineWithY : message::TextLine {
      TextLineWithY(int y, const message::TextLine &line) : y_(y), message::TextLine(line) {}
      int y_;
    };

    /// <summary>テキスト一行ごとの配列</summary>
    Array<TextLineWithY> text_line_;

    /// <summary>高さに収まらなくなった場合に飛び出した部分が入る</summary>
    Optional<message::Text> overflow_text;
  };
}
