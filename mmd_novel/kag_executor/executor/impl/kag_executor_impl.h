#pragma once
#include <queue>
#include <MmdNovel/kag_executor.h>

namespace kag
{
  class Executor::Pimpl
  {
    /// <summary>
    /// 内部でthisをキャプチャしてるのでクラスのコピー禁止
    /// <para>std::function&lt;void(Pimpl*)>とする手法があるがとりあえずこのまま</para>
    /// </summary>
    std::queue<std::function<void()>> command_;
    LayerManager layer_manager_;
    MessageManager message_manager_;
    ImageManager image_manager_;

    Pimpl(const Pimpl&) = delete;
    void operator=(const Pimpl&) = delete;

  public:

    Pimpl();

    ImageManager imageManager() { return image_manager_; }
    MessageManager messageManager() { return message_manager_; }

    void Clear();

    bool IsWait() const;

    void Command(const std::function<void()>& f);

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

    void CommandCurrent(int index, const LayerPage& page);

    void CommandIndent();

    void CommandEndIndent();

    void CommandLocate(Value<int> x, Value<int> y);

    void CommandStyle(const CommandFunc<StyleCommandEditor>& f);

    void CommandDefStyle(const CommandFunc<DefaultStyleCommandEditor>& f);

    void CommandResetStyle();

    void CommandFont(const CommandFunc<FontCommandEditor>& f);

    void CommandDefFont(const CommandFunc<DefFontCommandEditor>& f);

    void CommandResetFont();

    void CommandPosition(Value<int> layer, const Value<LayerPage>& page, const CommandFunc<PositionCommandEditor>& f);

    void CommandImage(int layer, const Value<LayerPage>& page, const Texture& tex);

    bool Update();

    bool CommandUpdate();

    void Draw();

  };
}
