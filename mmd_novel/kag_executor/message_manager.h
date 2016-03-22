#pragma once
#include <kag_tokenizer/tokenizer.h>
#include <kag_executor/message_layer.h>
namespace kag {
  enum LayerPage {
    Fore,
    Back
  };


  class MessageManager {
  public:

    MessageManager();

    /// <summary>
    /// メッセージレイヤのレイヤ数を変更する
    /// </summary>
    /// <param name="size">レイヤ数</param>
    void resize(size_t size);

    /// <summary>
    /// カレントレイヤをクリアする
    /// </summary>
    void Clear();

    /// <summary>
    /// すべてのレイヤをクリアする
    /// </summary>
    void AllClear();

    /// <summary>
    /// ディレイしてる文字がないときtrueを返す
    /// </summary>
    /// <returns></returns>
    bool IsFlush() const;

    /// <summary>
    /// ディレイしてる文字列をまとめて表示させる
    /// </summary>
    void Flush();

    /// <summary>
    /// 表示する文字列を末尾に追加する
    /// </summary>
    /// <param name="text"></param>
    void Append(const SnapShotSpan& text);

    /// <summary>
    /// 改行をする
    /// </summary>
    void AppendNewLine();

    /// <summary>
    /// 文字をクリアする
    /// </summary>
    void NextPage();

    void SetWaitClick() { is_wait_click_ = true; }
    void SetClickNextPage() { is_click_new_page = true; }

    /// <summary>
    /// メッセージレイヤーの更新をする
    /// <para>メッセージが貯まり改ページ待ちの時にfalseを返す</para>
    /// </summary>
    /// <returns></returns>
    bool Update();

    void Draw() const;

    /// <summary>
    /// カレントレイヤを変更する
    /// <para>表画面が指定される</para>
    /// </summary>
    /// <param name="layer_index"></param>
    void SetCurrentLayer(int layer_index);

    /// <summary>
    /// カレントレイヤを裏表画面の指定付きで変更する
    /// </summary>
    /// <param name="layer_index"></param>
    /// <param name="type"></param>
    void SetCurrent(int layer_index, LayerPage type);

    /// <summary>
    /// 1文字がディレイする時間を設定する
    /// </summary>
    /// <param name="delay_time"></param>
    void SetDelayTime(int delay_time);

    void SetNoWaitText(bool is_no_wait);

    void SetDefaultFont(const message::MessageTextFont& font) {
      default_font_ = font;
    }

    void SetFont(const message::MessageTextFont& font) {
      for (auto& i : message_layer_) {
        for (auto& j : i) {
          j.SetFont(font);
        }
      }
    }

    const message::MessageTextFont& NowFont() const { return now_font_; }
    /// <summary>
    /// メッセージレイヤを返す
    /// </summary>
    /// <param name="index">レイヤ番号</param>
    /// <param name="page">
    /// <para>0 : 表</para>
    /// <para>1 : 裏</para>
    /// </param>
    /// <returns></returns>
    MessageLayer& GetLayer(int index, int page) {
      return message_layer_[index][page];
    }

    /// <summary>
    /// カレントレイヤを返す
    /// </summary>
    /// <returns></returns>
    MessageLayer& Current() { return GetLayer(current_layer_, current_page_); }

    /// <summary>
    /// 現在のレイヤ番号を返す
    /// </summary>
    /// <returns></returns>
    int CurrentLayerNum() const { return current_layer_; }

    /// <summary>
    /// 現在の画面（裏 か 表）を返す
    /// </summary>
    /// <returns></returns>
    int CurrentPageNum() const { return current_page_; }

    bool IsWait() const { return is_wait_click_ || !IsFlush(); }
  private:

    Stopwatch timer_;

    /// <summary>
    /// 表が0、裏が1の配列の配列
    /// </summary>
    Array<std::array<MessageLayer, 2>> message_layer_;
    static constexpr int message_fore_layer = 0;
    static constexpr int message_back_layer = 1;

    /// <summary>
    /// 現在のレイヤ番号を表す
    /// </summary>
    int current_layer_;

    /// <summary>
    /// 現在のページを表す 表が0、裏が1
    /// </summary>
    int current_page_;

    /// <summary>遅延表示してるテキスト</summary>
    SnapShotSpan delay_text;

    /// <summary>次に表示する遅延テキストの添字</summary>
    int delay_index_;

    /// <summary>一文字あたりに遅延する時間</summary>
    int delay_time_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page;

    /// <summary>クリックによるスキップをするかどうか</summary>
    bool is_click_skip;

    bool is_no_wait_;

    message::MessageTextFont default_font_;

    message::MessageTextFont now_font_;
  };

}