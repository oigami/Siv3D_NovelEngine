#include <MmdNovel/imanager.h>
namespace kag
{
  IManager::IManager(const Executor& executor) :executor_(executor)
  {
  }

  PageLayer<LayerPtr>& IManager::GetLayer(int index)
  {
    return layer_[index];
  }

  LayerPtr & IManager::GetLayer(int index, LayerPage page)
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

  void IManager::resize(int num, const LayerPtr& fore, const LayerPtr& back)
  {
    const int pre_size = size();
    for ( int i = num; i < pre_size; i++ )
    {
      executor_.layerManager()->Remove(layer_[i]);
    }
    layer_.resize(num, { fore, back });
    for ( int i = pre_size; i < num; i++ )
    {
      executor_.layerManager()->Set(layer_[i]);
    }

  }

}
