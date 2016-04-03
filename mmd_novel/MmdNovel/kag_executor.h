#pragma once
#include <MmdNovel/message_manager.h>
#include <MmdNovel/tag_editor.h>
#include <MmdNovel/default_value.h>
namespace kag {

  /// <summary>
  /// kagの実行クラス
  /// <para>シャローコピーされるのでコピーをしてもデータが共有される</para>
  /// </summary>
  class Executor {
  public:

    Executor();

    /// <summary>
    /// 次のコマンドに進める時にtrueを返す
    /// </summary>
    /// <returns></returns>
    bool Update();
    bool CommandUpdate();

    void Draw();
    void Clear();

    bool IsWait() const;

    /* コマンド */

    void CommandL();

    void CommandR();

    void CommandP();

    void CommandDelay(int delay_time);

    void CommandNoWait();
    void CommandEndNoWait();

    void CommandER();

    void CommandCM();

    void CommandCT();

    /// <summary>
    /// テキストをメッセージレイヤに送る
    /// </summary>
    void CommandText(const SnapShotSpan& str);

    void CommandTextNoDelay(const SnapShotSpan& str);

    void CommandCurrent(int index, int page);

    void CommandIndent();
    void CommandEndIndent();

    void CommandLocate(Value<int> x, Value<int> y);

    template<class Editor>
    using CommandFunc = std::function<void(Editor&)>;

    void CommandStyle(const CommandFunc<StyleCommandEditor>& f);
    void CommandDefStyle(const CommandFunc<DefaultStyleCommandEditor>&f);
    void CommandResetStyle();

    void CommandFont(const CommandFunc<FontCommandEditor>& f);
    void CommandDefFont(const CommandFunc<DefFontCommandEditor>& f);

    void CommandResetFont();

    void CommandPosition(Value<int> layer, Value<int> page, const CommandFunc<PositionCommandEditor>& f);

  protected:
    void ShowErrorMsg(const String& str) const;

  protected:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;

  };
}
