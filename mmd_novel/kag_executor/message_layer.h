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
      MessageText() = default;

      MessageText(int x, const MessageTextFont& font, String&& text);

      MessageText(int x, const MessageTextFont& font);

      /// <summary>
      /// インデントをセットする
      /// </summary>
      /// <param name="x"></param>
      void Indent(int x);

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
      MessageText& Append(const String& s);

      /// <summary>
      /// 文字を末尾に追加する
      /// </summary>
      /// <param name="s">文字</param>
      /// <returns></returns>
      MessageText& Append(const wchar& s);

      /// <summary>
      /// 折り返し他部分を新しく生成して返す
      /// </summary>
      /// <param name="width">折り返す時の幅</param>
      /// <returns></returns>
      Optional<MessageText> ByReturn(int width);

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
      const MessageTextFont& Font() const;

    private:

      MessageTextFont font_;
      String text_;
      int start_x_;
    };

    class MessageTextLine {
    public:
      MessageTextLine(int y, const MessageText& text);

      void Clear();

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
      void Append(const MessageText& text);

      /// <summary>
      /// 折り返し部分を新たに生成して返す
      /// </summary>
      /// <param name="width">折り返す幅</param>
      /// <returns></returns>
      Optional<MessageText> ByReturn(int width);

      /// <summary>
      /// フォントを末尾に追加する
      /// <para>ここで指定したフォントがAppend(wchar),Append(String)で使用される</para>
      /// </summary>
      /// <param name="font"></param>
      void AppendNewFont(const MessageTextFont& font);

      void AppendNewFont(int x, const MessageTextFont & font);
      /// <summary>
      /// 最大の高さを返す
      /// </summary>
      /// <returns></returns>
      int Height() const;

      int Width() const;

      void SetLineSize(int px);

      void ResetLineSize();

      int LineSpacing() const;

      void SetLineSpacing(int px);
      void ResetLineSpacing();

    private:

      int y_;

      /// <summary>一行の中で最大の高さを保持する</summary>
      int max_height_;

      static constexpr int default_line_size = std::numeric_limits<int>::max();

      /// <summary>行（文字）の幅</summary>
      int line_size_;

      static constexpr int default_line_spacing = 0;


      /// <summary>行間</summary>
      int line_spacing_;

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
    const message::MessageTextFont& NowFont() const;

    /// <summary>
    /// フォントを設定する
    /// </summary>
    /// <param name="font"></param>
    void SetFont(const message::MessageTextFont& font);

    void SetDefaultFont(const message::MessageTextFont& font);

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

    void SetLocate(int x, int y);
    void SetLocateX(int x);
    void SetLocateY(int y);

    void SetIndent();
    void SetEndIndent();

    void SetLineSize(int px);
    void ResetLineSize();

    void SetLineSpacing(int px);
    void ResetLineSpacing();

  private:

    /// <summary>
    /// 折り返しのチェックをする
    /// <para>もし折り返して高さに収まらなくなった場合はoverflow_textに入れる</para>
    /// </summary>
    void CheckByReturn();

    static constexpr int InvalidIndent = std::numeric_limits<int>::max();

    /// <summary>インデントの開始位置(InvalidIndentで無効)</summary>
    int indent_width_;

    message::MessageTextFont now_font_;

    message::MessageTextFont default_font_;

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

    /// <summary>テキスト一行ごとの配列</summary>
    Array<message::MessageTextLine> text_line_;

    /// <summary>高さに収まらなくなった場合に飛び出した部分が入る</summary>
    Optional<message::MessageText> overflow_text;
  };

}
