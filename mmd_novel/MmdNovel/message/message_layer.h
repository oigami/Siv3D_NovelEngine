#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
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

    operator std::shared_ptr<Layer>() const;
  private:

    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;

  };

}
