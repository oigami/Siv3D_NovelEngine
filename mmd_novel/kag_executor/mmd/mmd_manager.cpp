#include "mmd_manager.h"
namespace kag
{
  MMDLayer MMDManager::GetLayer(int index, LayerPage page)
  {
    return IManager::GetLayer<MMDLayer>(index, page);
  }
}
