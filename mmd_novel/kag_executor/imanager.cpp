#include <MmdNovel/imanager.h>
#include <MmdNovel/layer/ilayer.h>
namespace kag
{
  IManager::IManager(const std::weak_ptr<Executor>& executor)
    :executor_(executor), layer_manager_(executor.lock()->layerManager())
  {
  }

  IManager::~IManager()
  {
    auto&& manager = layer_manager_;
    for ( auto& i : layer_ )
    {
      manager->Remove(i);
    }
  }

  PageLayer& IManager::GetLayer(int index)
  {
    return layer_[index];
  }

  ILayerPtr & IManager::GetLayer(int index, LayerPage page)
  {
    return GetLayer(index)[page];
  }

  int IManager::size() const
  {
    return static_cast<int>(layer_.size());
  }

  void IManager::Update()
  {
    update();
  }

  void IManager::resize(int num, const ILayerPtr& fore, const ILayerPtr& back)
  {
    const int pre_size = size();
    auto&& manager = layer_manager_;
    for ( int i = num; i < pre_size; i++ )
    {
      manager->Remove(layer_[i]);
    }
    layer_.resize(num);
    for ( int i = pre_size; i < num; i++ )
    {
      fore->clone(layer_[i].Fore());
      back->clone(layer_[i].Back());
      manager->Set(layer_[i]);
    }
  }

  std::shared_ptr<Executor> IManager::GetExecutor()
  {
    assert(!executor_.expired());
    return executor_.lock();
  }

}
