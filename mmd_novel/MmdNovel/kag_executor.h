﻿#pragma once
#include <MmdNovel/message/message_manager.h>
#include <MmdNovel/tag_editor.h>
#include <MmdNovel/default_value.h>
#include <kag_parser/kag_parser.h>
#include <MmdNovel/mmd/mmd_layer.h>

namespace kag
{
  struct IManager;
  using IManagerPtr = std::shared_ptr<IManager>;

  /// <summary>
  /// kagの実行クラス
  /// <para>シャローコピーされるのでコピーをしてもデータが共有される</para>
  /// </summary>
  class Executor
  {
  public:

    Executor();

    LayerPtr GetLayer(std::pair<kag::converter::LayerType, int> layer_num, LayerPage page) const
    {
      return GetLayer(layer_num)[page];
    }

    PageLayer<LayerPtr> GetLayer(std::pair<kag::converter::LayerType, int> layer_num) const;
    MessageManager messageManager()const;
    LayerManager layerManager()const;

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

    void AddManager(const SnapShotSpan& name, const IManagerPtr& manager);


    template<class layer, class...Args> std::shared_ptr<typename type_traits::GetType<layer>::type> MakeLayer(Args&&...args)
    {
      return std::make_shared<typename type_traits::GetType<layer>::type>(layerManager(), std::forward(args)...);
    }

  protected:

    void ShowErrorMsg(const String& str) const;

  protected:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;

    std::map<SnapShotSpan, IManagerPtr> manager_;


  };
}
