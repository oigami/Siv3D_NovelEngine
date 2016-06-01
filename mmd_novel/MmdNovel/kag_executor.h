#pragma once
#include <MmdNovel/message/message_manager.h>
#include <MmdNovel/image/image_manager.h>
#include <MmdNovel/tag_editor.h>
#include <MmdNovel/default_value.h>
#include <kag_parser/kag_parser.h>
namespace kag {

  /// <summary>
  /// kagの実行クラス
  /// <para>シャローコピーされるのでコピーをしてもデータが共有される</para>
  /// </summary>
  class Executor {
  public:

    Executor();

    LayerPtr GetLayer(std::pair<kag::converter::LayerType, int> layer_num, LayerPage page) {
      using namespace converter;
      if (layer_num.first == LayerType::Message) {
        return messageManager().GetLayer(layer_num.second, page);
      } else {
        return imageManager().GetLayer(layer_num.second, page);
      }
    }
    MessageManager messageManager();
    ImageManager imageManager();

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

    /// <summary>
    /// テキストをメッセージレイヤに送る
    /// </summary>
    void CommandText(const SnapShotSpan& str);

    void CommandTextNoDelay(const SnapShotSpan& str);

    void CommandCurrent(int index, const LayerPage& page);

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

    void CommandPosition(Value<int> layer, const Value<LayerPage>& page, const CommandFunc<PositionCommandEditor>& f);

    /* 画像関係 */

    void CommandImage(int layer, const Value<LayerPage>& page, const Texture& tex);

  protected:

    void ShowErrorMsg(const String& str) const;

  protected:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;

  };
}
