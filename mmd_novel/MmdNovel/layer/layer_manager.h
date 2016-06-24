#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer/page_layer.h>
namespace kag
{
  struct ILayer;
  using LayerPtr = std::shared_ptr<ILayer>;

  class LayerManagerImpl
  {
    Array<std::array<ILayer*, 2>> list_;

  public:
    void Update();

    void Draw() const;

    void Set(const PageLayer& layer);

    void Remove(const PageLayer& layer);

    void Update(const PageLayer& layer);

    void Sort();
  };
}
