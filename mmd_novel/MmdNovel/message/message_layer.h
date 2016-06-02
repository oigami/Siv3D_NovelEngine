#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
#include <MmdNovel/message/message_layer_impl.h>
namespace kag {
  namespace message {
    namespace pimpl {
      // TODO: opacityを全体に反映させるための処理を追加する
      class MessageLayerPimpl : public Layer {
      public:
        static std::shared_ptr<MessageLayerPimpl> create() {
          return std::make_shared<MessageLayerPimpl>();
        }
        MessageLayerPimpl() {
          SetPositionLeft(16);
          SetPositionTop(16);
          SetPositionWidth(Window::Size().x - 32);
          SetPositionHeight(Window::Size().y - 32);
          IsVisible(false);
          margin_.set({ 8,8 }, { 8,8 });
          background_color_ = Palette::Gray;
          background_color_.a = 128;
          indent_width_ = InvalidIndent;
          Clear();
        }
        /// <summary>
        /// メッセージボックスをクリアする
        /// </summary>
        void Clear() {
          limit_line_num = 0;
          NextPage();
        }
        /// <summary>
        /// 次のページに行く
        /// </summary>
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
        /// <summary>
        /// 文字列を末尾に追加する
        /// </summary>
        /// <param name="str"></param>
        void Append(const String & str) {
          text_line_.back().Append(str);
          CheckByReturn();
        }
        /// <summary>
        /// 文字を末尾に追加する
        /// </summary>
        /// <param name="str"></param>
        void Append(const wchar & str) {
          text_line_.back().Append(str);

          //Print(str);
          CheckByReturn();
        }
        /// <summary>
        /// 文字列が縦方向に収まらなくなった時trueを返す
        /// </summary>
        /// <returns></returns>
        bool IsLimitHeihgt() {
          return position().h - margin_.h - margin_.y <= sum_height_ + text_line_.back().Height();
        }
        /// <summary>
        /// 改行をする
        /// </summary>
        void AppenNewLine() {
          sum_height_ += text_line_.back().Height();
          text_line_.emplace_back(sum_height_, message::Text(now_font_));
          if (!IsLimitHeihgt()) {
            limit_line_num = static_cast<int>(text_line_.size());
          }
        }
        /// <summary>
        /// 現在指定されているフォントを返す
        /// </summary>
        /// <returns></returns>
        void draw() const {
          auto& pos = position();
          pos.draw(background_color_);
          if (!background_tex_.isEmpty())
            pos(background_tex_).draw();

          int y = position().y + margin_.y;
          for (auto& i : step(limit_line_num)) {
            text_line_[i].Draw(pos.x + margin_.x, y + text_line_[i].y_);
          }
        }

        const kag::message::TextFont & NowFont() const { return now_font_; }
        /// <summary>
        /// フォントを設定する
        /// </summary>
        /// <param name="font"></param>
        void SetFont(const message::TextFont & font) {
          text_line_.back().AppendNewFont(font);
          now_font_ = font;
        }

        /// <summary>
        /// デフォルトフォントを変更する
        /// </summary>
        /// <param name="font">設定するフォント</param>
        void SetDefaultFont(const message::TextFont & font) {
          default_font_ = font;
        }

        /// <summary>
        /// デフォルトのフォントに戻す
        /// </summary>
        void ResetFont() {
          SetFont(default_font_);
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
        /// <summary>
        /// 文字列の開始位置を変更する
        /// </summary>
        /// <param name="x">開始するx座標</param>
        /// <param name="y">開始するy座標</param>
        void SetLocate(int x, int y) {
          text_line_.emplace_back(y, message::TextLine(x, now_font_));
          sum_height_ = y;
        }
        /// <summary>
        /// 文字列の開始位置を変更する
        /// </summary>
        /// <param name="x"></param>
        void SetLocateX(int x) {
          text_line_.back().AppendNewFont(x, now_font_);
        }
        /// <summary>
        /// 文字列の開始位置を変更する
        /// </summary>
        /// <param name="y"></param>
        void SetLocateY(int y) {
          SetLocate(0, y);
        }
        /// <summary>
        /// 現在の文字列を基準にインデントを開始する
        /// </summary>
        void BeginIndent() {
          indent_width_ = text_line_.back().Width();
        }
        /// <summary>
        /// インデントを終了する
        /// </summary>
        void EndIndent() {
          indent_width_ = InvalidIndent;
        }
        /// <summary>
        /// 行間（文字を描画する縦幅）を設定する
        /// <para>デフォルトでは文字の縦幅が一番大きい値になっている</para>
        /// <para>ここで指定したサイズより大きい文字を描画すると収まらなくなる</para>
        /// </summary>
        /// <param name="px"></param>
        void SetLineSize(int px) {
          text_line_.back().SetLineSize(px);
        }

        /// <summary>
        /// 行間をリセットしデフォルト値に戻す
        /// </summary>
        void ResetLineSize() {
          text_line_.back().ResetLineSize();
        }
        /// <summary>
        /// 行と行の間のサイズ（字の大きさを無視した値）を設定する
        /// <para>実際はここで指定した値とlinesizeが加算された値になる</para>
        /// </summary>
        /// <param name="px"></param>
        void SetLineSpacing(int px) {
          text_line_.back().SetLineSpacing(px);
        }
        /// <summary>
        /// 行と行の間のサイズをリセットする
        /// </summary>
        void ResetLineSpacing() {
          text_line_.back().ResetLineSpacing();
        }
        /// <summary>
        /// デフォルトのスタイルを設定する
        /// </summary>
        /// <param name="style"></param>
        void SetDefaultStyle(const message::DefaultStyle & style) {
          default_style_ = style;
        }
        /// <summary>
        /// 現在のスタイルをデフォルトのスタイルに変更する
        /// </summary>
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
            auto opt = text_line_.back().ByReturn(position().w - margin_.x - margin_.w);
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

        /// <summary>
        /// メッセージを表示する部分のマージン
        /// <para>rect型だがposition_からの相対位置(内側が正)になっている</para>
        /// </summary>
        Rect margin_;

        /// <summary>背景テクスチャ</summary>
        Texture background_tex_;

        /// <summary>背景色</summary>
        Color background_color_;

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
  }

  using MessageLayer = LayerHelper<message::pimpl::MessageLayerPimpl>;
}
