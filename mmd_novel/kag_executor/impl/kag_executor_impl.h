#pragma once
#include <MmdNovel/kag_executor.h>

namespace kag {
  class Executor::Pimpl {
    /// <summary>
    /// 内部でthisをキャプチャしてるのでクラスのコピー禁止
    /// <para>std::function&lt;void(Pimpl*)>とする手法があるがとりあえずこのまま</para>
    /// </summary>
    std::queue<std::function<void()>> command_;
    MessageManager message_manager_;

    Pimpl(const Pimpl&) = delete;
    void operator=(const Pimpl&) = delete;

  public:

    Pimpl();

    void Clear();

    bool IsWait() const;

    void CommandL();

    void CommandR();

    void CommandP();

    void CommandDelay(int delay_time);

    void CommandNoWait();

    void CommandEndNoWait();

    void CommandER();

    void CommandCM();

    void CommandCT();

    void CommandText(const SnapShotSpan& str);

    void CommandTextNoDelay(const SnapShotSpan & str);

    void CommandCurrent(int index, int page);

    void CommandIndent();

    void CommandEndIndent();

    void CommandLocate(Value<int> x, Value<int> y);

    void CommandStyle(const CommandFunc<StyleCommandEditor>& f);

    void CommandDefStyle(const CommandFunc<DefaultStyleCommandEditor>& f);

    void CommandResetStyle();

    void CommandFont(const CommandFunc<FontCommandEditor>& f);

    void CommandDefFont(const CommandFunc<DefFontCommandEditor>& f);

    void CommandResetFont();

    void CommandPosition(Value<int> layer, Value<int> page, const CommandFunc<PositionCommandEditor>& f);

    bool Update();

    bool CommandUpdate();

    void Draw();

  };
}
