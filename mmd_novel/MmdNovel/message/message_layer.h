#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer/ilayer.h>
#include <MmdNovel/message/message_text.h>
namespace kag
{
  namespace message
  {
    namespace pimpl
    {
      // TODO: opacityを全体に反映させるための処理を追加する
      class MessageLayerPimpl : public Layer
      {
        virtual void clone(LayerPtr& out);
      public:

        MessageLayerPimpl(const LayerManager& manager);

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
        void Append(const String & str);

        /// <summary>
        /// 文字を末尾に追加する
        /// </summary>
        /// <param name="str"></param>
        void Append(const wchar & str);

        /// <summary>
        /// 文字列が縦方向に収まらなくなった時trueを返す
        /// </summary>
        /// <returns></returns>
        bool IsLimitHeihgt();

        /// <summary>
        /// 改行をする
        /// </summary>
        void AppenNewLine();

        void draw() const;

        /// <summary>
        /// 現在指定されているフォントを返す
        /// </summary>
        /// <returns></returns>
        const kag::message::TextFont & NowFont() const;

        /// <summary>
        /// フォントを設定する
        /// </summary>
        /// <param name="font"></param>
        void SetFont(const message::TextFont & font);

        /// <summary>
        /// デフォルトフォントを変更する
        /// </summary>
        /// <param name="font"> 設定するフォント </param>
        void SetDefaultFont(const message::TextFont & font);

        /// <summary>
        /// デフォルトのフォントに戻す
        /// </summary>
        void ResetFont();

        void SetMarginTop(int top);

        void SetMarginLeft(int left);

        void SetMarginRight(int width);

        void SetMarginBottom(int height);

        void SetBackgroundColor(Color argb);

        void SetBackgroundRGB(int r, int g, int b);

        void SetBackgroundOpacity(int a);

        void SetBackgroundTex(Texture tex);

        /// <summary>
        /// 文字列の開始位置を変更する
        /// </summary>
        /// <param name="x"> 開始するx座標 </param>
        /// <param name="y"> 開始するy座標 </param>
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
        void BeginIndent()
        {
          indent_width_ = text_line_.back().Width();
        }

        /// <summary>
        /// インデントを終了する
        /// </summary>
        void EndIndent();

        /// <summary>
        /// 行間（文字を描画する縦幅）を設定する
        /// <para> デフォルトでは文字の縦幅が一番大きい値になっている </para>
        /// <para> ここで指定したサイズより大きい文字を描画すると収まらなくなる </para>
        /// </summary>
        /// <param name="px"></param>
        void SetLineSize(int px);

        /// <summary>
        /// 行間をリセットしデフォルト値に戻す
        /// </summary>
        void ResetLineSize();

        /// <summary>
        /// 行と行の間のサイズ（字の大きさを無視した値）を設定する
        /// <para> 実際はここで指定した値とlinesizeが加算された値になる </para>
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
        void SetDefaultStyle(const message::DefaultStyle & style);

        /// <summary>
        /// 現在のスタイルをデフォルトのスタイルに変更する
        /// </summary>
        void ResetStyle();
      private:

        /// <summary>
        /// 折り返しのチェックをする
        /// <para> もし折り返して高さに収まらなくなった場合はoverflow_textに入れる </para>
        /// </summary>
        void CheckByReturn();

        //インデントが無効な時の値
        static constexpr int InvalidIndent = std::numeric_limits<int>::max();

        // インデントの開始位置(InvalidIndentで無効)
        int indent_width_;

        // 現在設定されているフォント
        message::TextFont now_font_;

        // デフォルトに設定されているフォント
        message::TextFont default_font_;

        // デフォルトに設定されているフォント
        message::DefaultStyle default_style_;

        // メッセージを表示する部分のマージン rect型だがposition_からの相対位置(内側が正)になっている
        Rect margin_;

        // 背景テクスチャ
        Texture background_tex_;

        // 背景色
        Color background_color_;

        // 描画してる文字列の高さの合計
        int sum_height_;

        // 表示できた限界の行数
        int limit_line_num;

        struct TextLineWithY : message::TextLine
        {
          TextLineWithY(int y, const message::TextLine &line) : y_(y), message::TextLine(line) {}
          int y_;
        };

        // テキスト一行ごとの配列
        Array<TextLineWithY> text_line_;

        // 高さに収まらなくなった場合に飛び出した部分が入る
        Optional<message::Text> overflow_text;
      };
    }
  }

  using MessageLayer = std::shared_ptr<message::pimpl::MessageLayerPimpl>;
}
