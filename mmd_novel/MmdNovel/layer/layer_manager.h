#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer/page_layer.h>
namespace kag
{
  struct ILayer;
  using LayerPtr = std::shared_ptr<ILayer>;

  class LayerManagerImpl
  {
    Array<PageLayer<LayerPtr>> list_;

  public:
    void Update();

    void Draw() const;

    void Set(const PageLayer<LayerPtr>& layer);

    void Remove(const PageLayer<LayerPtr>& layer);

    void Update(const PageLayer<LayerPtr>& layer);

    void Sort();
  };
}
