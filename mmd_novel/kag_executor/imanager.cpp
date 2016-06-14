#include <MmdNovel/imanager.h>
namespace kag
{
  PageLayer<LayerPtr>& IManager::GetLayer(int index)
  {
    return layer_[index];
  }

  LayerPtr & IManager::GetLayer(int index, LayerPage page)
  {
    return GetLayer(index)[page];
  }

  void IManager::resize(int size, PageLayer<LayerPtr>& val)
  {
    layer_.resize(size, val);
  }

}
