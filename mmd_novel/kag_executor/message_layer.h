#pragma once
#include <Siv3D.hpp>

namespace kag {
  namespace message {
    struct MessageTextFont {
      Font font_;
      Color color_;
      MessageTextFont() :color_(Palette::White) {}
      MessageTextFont(const Font& font, Color color) :font_(font), color_(color) {
      }

      RectF Draw(const String& str, int x, int y) const {
        return font_.draw(str, x, y - font_.height, color_);
      }

      size_t drawableCharacters(const String& str, int width) const {
        return font_.drawableCharacters(str, width);
      }

      Rect region(const String& str) const {
        return font_(str).region();
      }

      int Height() const { return font_.height; }

    };

    class MessageText {
    public:
      MessageText() {
      }

      MessageText(int x, const MessageTextFont& font, String&& text)
        : start_x_(x), font_(font), text_(std::move(text)) {
      }

      MessageText(int x, const MessageTextFont& font)
        : start_x_(x), font_(font) {
      }

      /// <summary>
      /// 文字列を描画する
      /// </summary>
      /// <param name="x">表示するx座標</param>
      /// <param name="y_add_fontheight">表示するy座標にfont.heightを足した座標</param>
      /// <returns></returns>
      RectF Draw(int x, int y_add_fontheight) const {
        return font_.Draw(text_, x, y_add_fontheight);
      }

      MessageText& Append(const String& s) {
        text_.append(s);
        return *this;
      }
      MessageText& Append(const wchar& s) {
        text_ += s;
        return *this;
      }

      /// <summary>
      /// 折り返し他部分を新しく生成して返す
      /// </summary>
      /// <param name="width">折り返す時の幅</param>
      /// <returns></returns>
      Optional<MessageText> ByReturn(int width) {

        // すでに使用している分を引いて使える部分の幅のみで計算する
        size_t index = font_.drawableCharacters(text_, width - start_x_);
        if (text_.length == index) return none;
        String s = text_.substr(index);
        text_.resize(index);
        return MessageText(0, font_, std::move(s));
      }

      int GetWidth() const {
        return start_x_ * font_.region(text_).w;
      }

      int Height() const {
        return font_.Height();
      }

      const MessageTextFont& Font() const { return font_; }
    private:

      MessageTextFont font_;
      String text_;
      int start_x_;
    };

    class MessageTextLine {
    public:
      MessageTextLine(const int x, MessageTextFont& font);
      MessageTextLine(const MessageText& text);

      void Clear();

      int Draw(int x, int y) const;

      void Append(const String& str);
      void Append(const wchar& str);
      void Append(const MessageText& text);

      Optional<MessageText> ByReturn(int width);

      void AppendNewFont(const MessageTextFont& font);

      /// <summary>
      /// 最大の高さを返す
      /// </summary>
      /// <returns></returns>
      int Height() const {
        return max_height_;
      }

    private:

      /// <summary>一行の中で最大の高さを保持する</summary>
      int max_height_;

      Array<MessageText> text_;

    };
  }
  class MessageLayer {
  public:

    MessageLayer();

    /// <summary>
    /// メッセージボックスをクリアする
    /// </summary>
    void Clear();

    void NextPage();

    /// <summary>
    /// 文字列を追加する
    /// </summary>
    /// <param name="str"></param>
    void Append(const String& str);

    void Append(const wchar& str);

    bool IsLimitHeihgt();

    /// <summary>
    /// 改行を末尾に追加する
    /// </summary>
    void AppenNewLine();

    void Draw() const;

  private:
    void CheckByReturn();

    message::MessageTextFont now_font_;
    Rect position_;
    bool is_visible_;

    int now_height;

    /// <summary>表示できた限界の行数</summary>
    int limit_line_num;

    Array<message::MessageTextLine> text_line_;

    Optional<message::MessageText> overflow_text;
  };

}
