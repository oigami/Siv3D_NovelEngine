#pragma once
#include <MmdNovel/message/snapshotspan.h>
#include <MmdNovel/message/message_layer.h>
#include <MmdNovel/default_value.h>
#include <MmdNovel/layer.h>

namespace kag {

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

    void SetWaitClick();
    void SetClickNextPage();

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

    /// <summary>
    /// メッセージレイヤを返す
    /// </summary>
    /// <param name="index">レイヤ番号</param>
    /// <param name="page">
    /// <para>0 : 表</para>
    /// <para>1 : 裏</para>
    /// </param>
    /// <returns></returns>
    MessageLayer& GetLayer(int index, int page);

    /// <summary>
    /// カレントレイヤを返す
    /// </summary>
    /// <returns></returns>
    MessageLayer& Current();

    /// <summary>
    /// 現在のレイヤ番号を返す
    /// </summary>
    /// <returns></returns>
    int CurrentLayerNum() const;

    /// <summary>
    /// 現在の画面（裏 か 表）を返す
    /// </summary>
    /// <returns></returns>
    int CurrentPageNum() const;

    /// <summary>
    /// クリックに対応するキーを設定する
    /// </summary>
    /// <param name="key">Input::MouseL | Input::KeyEnter 等</param>
    void SetClickKey(const KeyCombination& key);

    /// <summary>
    /// キー入力を無効にする
    /// <para>クリック待ちなどでも進まなくなる</para>
    /// </summary>
    void SetInvalidKeyInput();

    /// <summary>
    /// キー入力を有効にする
    /// <para>クリック待ちなどでクリックすると進むようになる</para>
    /// </summary>
    void SetValidKeyInput();

    void SetLayerManager(LayerManager& manager);

    bool IsWait() const;

  private:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;

  };

}
