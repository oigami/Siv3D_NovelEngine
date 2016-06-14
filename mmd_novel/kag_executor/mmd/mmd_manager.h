#pragma once
#include <MmdNovel/imanager.h>
#include <MmdNovel/mmd/mmd_layer.h>
namespace kag
{
  class MMDManager : IManager
  {
    static const PageLayer<MMDLayer> default_layer;
  public:

    MMDManager();

    void resize(int num);

    MMDLayer GetLayer(int index, LayerPage page);

  };
}
