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

    MessageLayer& GetLayer(int index, int page) {
      return message_layer_[index][page];
    }

    /// <summary>
    /// カレントレイヤを返す
    /// </summary>
    /// <returns></returns>
    MessageLayer& Current() { return GetLayer(current_layer_, current_page_); }

    int CurrentLayerNum() const { return current_layer_; }
    int CurrentPageNum() const { return current_page_; }

  private:

    Stopwatch timer_;

    /// <summary>
    /// 表が0、裏が1の配列の配列
    /// </summary>
    Array<std::array<MessageLayer, 2>> message_layer_;
    static constexpr int message_fore_layer = 0;
    static constexpr int message_back_layer = 1;

    int current_layer_;

    /// <summary>
    /// 現在のページを表す 表が0、裏が1
    /// </summary>
    int current_page_;

    SnapShotSpan delay_text;
    int delay_pos;

    int delay_time_;

  };

}