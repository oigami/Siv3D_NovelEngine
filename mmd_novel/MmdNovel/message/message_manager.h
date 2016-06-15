#pragma once
#include <MmdNovel/kag_file_executor.h>
#include <MmdNovel/message/snapshotspan.h>
#include <MmdNovel/message/message_layer.h>
#include <MmdNovel/default_value.h>
#include <MmdNovel/layer.h>
namespace kag
{
  class MessageManager : public IFileManager, std::enable_shared_from_this<MessageManager>
  {
    virtual void AddTag(FuncList& list);

  public:

    MessageManager(const Executor& exe);

    /// <summary>
    /// メッセージレイヤのレイヤ数を変更する
    /// </summary>
    /// <param name="size">レイヤ数</param>
    void resize(size_t size);

    int size() const;

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
    /// <param name="page"> ページ番号 </param>
    /// <returns></returns>
    MessageLayer& GetLayer(int index, LayerPage page);

    /// <summary>
    /// メッセージレイヤを返す
    /// </summary>
    /// <param name="index">レイヤ番号</param>
    /// <returns></returns>
    PageLayer<MessageLayer>& GetLayer(int index);

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
    LayerPage CurrentPage() const;

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



    /* タグ一覧 */


    //void CancelAutoModeTag(CommandToken& token);
    //void CancelSkipTag(CommandToken& token);
    void CHTag(CommandToken& token);
    void CMTag(CommandToken& token);
    void CTTag(CommandToken& token);
    void CurrentTag(CommandToken& token);
    void DefFontTag(CommandToken& token);
    void DefStyleTag(CommandToken& token);
    void DelayTag(CommandToken& token);
    void EndIndentTag(CommandToken& token);
    void EndNoWaitTag(CommandToken& token);
    void ERTag(CommandToken& token);
    void FontTTag(CommandToken& token);

    //void GlyphTag(CommandToken& token);
    //void GraphTag(CommandToken& token);
    //void HCHTag(CommandToken& token);
    void IndentTag(CommandToken& token);
    void LTag(CommandToken& token);
    void LocateTag(CommandToken& token);

    //void LockLinkTag(CommandToken& token);
    void NoWaitTag(CommandToken& token);
    void PTag(CommandToken& token);
    void PositionTTag(CommandToken& token);
    void RTag(CommandToken& token);
    void ResetFontTag(CommandToken& token);
    void ResetStyleTag(CommandToken& token);

    //void RubyTag(CommandToken& token);
    void StyleTag(CommandToken& token);

  private:

    bool CheckClicked() const;

    Stopwatch timer_;

    /// <summary>
    /// 表が0、裏が1の配列の配列
    /// </summary>
    Array<PageLayer<MessageLayer>> message_layer_;
    static constexpr int message_fore_layer = 0;
    static constexpr int message_back_layer = 1;

    /// <summary>
    /// 現在のレイヤ番号を表す
    /// </summary>
    int current_layer_;

    /// <summary>
    /// 現在のページを表す
    /// </summary>
    LayerPage current_page_;

    /// <summary>遅延表示してるテキスト</summary>
    SnapShotSpan delay_text_;

    /// <summary>次に表示する遅延テキストの添字</summary>
    int delay_index_;

    /// <summary>一文字あたりに遅延する時間</summary>
    int delay_time_;

    /// <summary>クリック待ちするかどうか</summary>
    bool is_wait_click_;

    /// <summary>クリックした時に改ページに行くかどうか</summary>
    bool is_click_new_page_;

    /// <summary>クリックによるスキップをするかどうか</summary>
    bool is_click_skip_;

    bool is_no_wait_;

    /// <summary>クリックに対応するキー</summary>
    KeyCombination click_key_;

    /// <summary>キー入力が有効かどうか</summary>
    bool is_active_key_;

    LayerManager layer_manager_;
  };

}
