#pragma once
#include <MmdNovel/layer.h>
namespace kag
{
  namespace message
  {
    enum AlignType
    {
      Left,
      Center,
      Right,
    };
    struct DefaultStyle
    {
      static constexpr int default_line_size = std::numeric_limits<int>::max();
      static constexpr int default_line_spacing = 0;

      DefaultStyle() :line_spacing_(0), line_size_(default_line_size) {}

      bool IsDefaultLineSize() const { return line_size_ == default_line_size; }

      int LineSize(int default_size) const
      {
        return IsDefaultLineSize() ? default_size : line_size_;
      }
      void ResetLineSize()
      {
        line_size_ = default_line_size;
      }
      void ResetLineSpacing()
      {
        line_spacing_ = default_line_spacing;
      }

      /// <summary>行（文字）の幅</summary>
      int line_spacing_;

      /// <summary>行間</summary>
      int line_size_;

      //TODO: 未実装
      int pitch_;
    };

    struct Style : DefaultStyle
    {
      //TODO: 未実装
      AlignType align_type_;
      bool auto_return_;
    };

    struct TextFont
    {
      Font font_;
      Color color_;
      Color shadow_color_;
      bool is_shadow_;

      //TODO: フォントのサイズに応じて可変にする
      Point shadow_pos = { 5,5 };
      TextFont() :TextFont(Font(), Palette::White) {}
      TextFont(const Font& font, const Color& color) :font_(font), color_(color), shadow_color_(Palette::Gray), is_shadow_(true)
      {
      }

      RectF Draw(const String& str, int x, int y) const
      {
        if ( is_shadow_ ) font_.draw(str, x + shadow_pos.x, y - font_.height + shadow_pos.x, shadow_color_);
        return font_.draw(str, x, y - font_.height, color_);
      }

      size_t drawableCharacters(const String& str, int width) const
      {
        return font_.drawableCharacters(str, width);
      }

      Rect region(const String& str) const
      {
        return font_(str).region();
      }

      int Height() const { return font_.height; }

    };

    /// <summary>
    /// フォント付き修飾テキスト
    /// </summary>
    class Text
    {
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

    class TextLine
    {
    private:
      struct TextWithX
      {
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
}
