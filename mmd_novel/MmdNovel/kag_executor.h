#pragma once
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
    LayerManager layerManager()const;

    template<class Manager>typename  type_traits::GetType<Manager>::shared GetManager(const SnapShotSpan& name)
    {
      return std::dynamic_pointer_cast<typename type_traits::GetType<Manager>::type>(manager_[name]);
    }

    /// <summary>
    /// 次のコマンドに進める時にtrueを返す
    /// </summary>
    /// <returns></returns>
    bool Update();
    bool CommandUpdate();

    void Draw();
    void Clear();

    bool IsWait();

    /* コマンド */

    void Command(const std::function<void()>& f);

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

    bool is_wait_;

  };
}
