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

  void IManager::resize(int num, const LayerPtr& fore, const LayerPtr& back)
  {
    layer_.resize(num, { fore, back });
  }

}
