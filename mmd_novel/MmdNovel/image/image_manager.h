#pragma once
#include <MmdNovel/image/image_layer.h>
#include <MmdNovel/default_value.h>
namespace kag {
  class ImageManager {
  public:
    ImageManager() :layer_(2) {
      for (auto& i : step(layer_.size())) {
        const short index = static_cast<short>((i + 1) * 10);
        layer_[i][Define::fore_page].SetZIndex(index);
        layer_[i][Define::back_page].SetZIndex(index);
      }
    }

    ImageLayer& GetLayer(int layer, LayerPage page) { return layer_[layer][page]; }

    void SetLayerManager(LayerManager& manager) {
      layer_manager_ = manager;
      for (auto& i : layer_) {
        layer_manager_->Set(i[0]);
      }
    }
  private:

    LayerManager layer_manager_;
    Array<std::array<ImageLayer, 2>> layer_;
  };
}
