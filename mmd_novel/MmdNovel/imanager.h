#pragma once
#include <MmdNovel/layer.h>
namespace kag
{
  class IManager
  {
    IManager() = default;

  public:

    virtual ~IManager() = default;

    PageLayer<LayerPtr>& GetLayer(int index);

    LayerPtr& GetLayer(int index, LayerPage page);

    void resize(int size, PageLayer<LayerPtr>& val);

  private:

    Array<PageLayer<LayerPtr>> layer_;
  };
}
