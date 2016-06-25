#pragma once
#include <MmdNovel/kag_token.h>
#include <MmdNovel/layer/page_layer.h>
#include <MmdNovel/layer/layer_manager.h>
#include <MmdNovel/type_traits.h>
namespace kag
{
  struct IManager;
  using IManagerPtr = std::shared_ptr<IManager>;
  struct ILayer;
  using ILayerPtr = std::shared_ptr<ILayer>;
  class LayerPage;

  /// <summary>
  /// kagの実行クラス
  /// <para>シャローコピーされるのでコピーをしてもデータが共有される</para>
  /// </summary>
  class Executor : public std::enable_shared_from_this<Executor>
  {
  public:

    Executor();

    ILayerPtr GetLayer(std::pair<LayerType, int> layer_num, const LayerPage& page) const
    {
      return GetLayer(layer_num)[page];
    }

    PageLayer& GetLayer(std::pair<LayerType, int> layer_num) const;
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
