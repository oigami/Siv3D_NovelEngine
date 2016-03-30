#pragma once
#include <Siv3D.hpp>

namespace kag {
  namespace message {
    enum AlignType {
      Left,
      Center,
      Right,
    };
    struct DefaultStyle {
      static constexpr int default_line_size = std::numeric_limits<int>::max();
      static constexpr int default_line_spacing = 0;

      DefaultStyle() :line_spacing_(0), line_size_(default_line_size) {}

      bool IsDefaultLineSize() const { return line_size_ == default_line_size; }

      int LineSize(int default_size) const {
        return IsDefaultLineSize() ? default_size : line_size_;
      }
      void ResetLineSize() {
        line_size_ = default_line_size;
      }
      void ResetLineSpacing() {
        line_spacing_ = default_line_spacing;
      }

      /// <summary>行（文字）の幅</summary>
      int line_spacing_;

      /// <summary>行間</summary>
      int line_size_;

      //TODO: 未実装
      int pitch_;
    };

    struct Style : DefaultStyle {

      //TODO: 未実装
      AlignType align_type_;
      bool auto_return_;
    };
    struct TextFont {
      Font font_;
      Color color_;
      Color shadow_color_;
      bool is_shadow_;
      //TODO: フォントのサイズに応じて可変にする
      Point shadow_pos = { 5,5 };
      TextFont() :TextFont(Font(), Palette::White) {}
      TextFont(const Font& font, const Color& color) :font_(font), color_(color), shadow_color_(Palette::Gray), is_shadow_(true) {
      }

      RectF Draw(const String& str, int x, int y) const {
        if (is_shadow_) font_.draw(str, x + shadow_pos.x, y - font_.height + shadow_pos.x, shadow_color_);
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
  class MessageLayer {
  public:

    MessageLayer();

    /// <summary>
    /// メッセージボックスをクリアする
    /// </summary>
    void Clear();

    /// <summary>
    /// 次のページに行く
    /// </summary>
    void NextPage();

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
    /// 文字列が縦方向に収まらなくなった時trueを返す
    /// </summary>
    /// <returns></returns>
    bool IsLimitHeihgt();

    /// <summary>
    /// 改行をする
    /// </summary>
    void AppenNewLine();

    void Draw() const;

    /// <summary>
    /// 現在指定されているフォントを返す
    /// </summary>
    /// <returns></returns>
    const message::TextFont& NowFont() const;

    /// <summary>
    /// フォントを設定する
    /// </summary>
    /// <param name="font"></param>
    void SetFont(const message::TextFont& font);

    /// <summary>
    /// デフォルトフォントを変更する
    /// </summary>
    /// <param name="font">設定するフォント</param>
    void SetDefaultFont(const message::TextFont& font);

    /// <summary>
    /// デフォルトのフォントに戻す
    /// </summary>
    void ResetFont();

    void SetPositionTop(int top);
    void SetPositionLeft(int left);
    void SetPositionWidth(int width);
    void SetPositionHeight(int height);

    void SetMarginTop(int top);
    void SetMarginLeft(int left);
    void SetMarginRight(int width);
    void SetMarginBottom(int height);

    void SetVisible(bool is_visible);

    void SetBackgroundColor(Color argb);
    void SetBackgroundRGB(int r, int g, int b);
    void SetBackgroundOpacity(int a);
    void SetBackgroundTex(Texture tex);

    /// <summary>
    /// 文字列の開始位置を変更する
    /// </summary>
    /// <param name="x">開始するx座標</param>
    /// <param name="y">開始するy座標</param>
    void SetLocate(int x, int y);

    /// <summary>
    /// 文字列の開始位置を変更する
    /// </summary>
    /// <param name="x"></param>
    void SetLocateX(int x);

    /// <summary>
    /// 文字列の開始位置を変更する
    /// </summary>
    /// <param name="y"></param>
    void SetLocateY(int y);

    /// <summary>
    /// 現在の文字列を基準にインデントを開始する
    /// </summary>
    void BeginIndent();

    /// <summary>
    /// インデントを終了する
    /// </summary>
    void EndIndent();

    /// <summary>
    /// 行間（文字を描画する縦幅）を設定する
    /// <para>デフォルトでは文字の縦幅が一番大きい値になっている</para>
    /// <para>ここで指定したサイズより大きい文字を描画すると収まらなくなる</para>
    /// </summary>
    /// <param name="px"></param>
    void SetLineSize(int px);

    /// <summary>
    /// 行間をリセットしデフォルト値に戻す
    /// </summary>
    void ResetLineSize();

    /// <summary>
    /// 行と行の間のサイズ（字の大きさを無視した値）を設定する
    /// <para>実際はここで指定した値とlinesizeが加算された値になる</para>
    /// </summary>
    /// <param name="px"></param>
    void SetLineSpacing(int px);

    /// <summary>
    /// 行と行の間のサイズをリセットする
    /// </summary>
    void ResetLineSpacing();

    /// <summary>
    /// デフォルトのスタイルを設定する
    /// </summary>
    /// <param name="style"></param>
    void SetDefaultStyle(const message::DefaultStyle& style);

    /// <summary>
    /// 現在のスタイルをデフォルトのスタイルに変更する
    /// </summary>
    void ResetStyle();

  private:

    /// <summary>
    /// 折り返しのチェックをする
    /// <para>もし折り返して高さに収まらなくなった場合はoverflow_textに入れる</para>
    /// </summary>
    void CheckByReturn();

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
      TextLineWithY(int y, const message::TextLine &line) : y_(y) ,message::TextLine(line){}
      int y_;
    };

    /// <summary>テキスト一行ごとの配列</summary>
    Array<TextLineWithY> text_line_;

    /// <summary>高さに収まらなくなった場合に飛び出した部分が入る</summary>
    Optional<message::Text> overflow_text;
  };

}
