#pragma once
#include <MmdNovel/image/image_layer.h>
#include <MmdNovel/default_value.h>
namespace kag {
  class ImageManager {
  public:
    ImageManager() :layer_(2) {
      for (auto& i : step(layer_.size())) {
        const short index = static_cast<short>((i + 1) * 10);
        layer_[i][LayerPage::Fore]->SetZIndex(index);
        layer_[i][LayerPage::Back]->SetZIndex(index);
      }
    }

    ImageLayer& GetLayer(int layer, LayerPage page) { return layer_[layer][page]; }

    void SetLayerManager(LayerManager& manager) {
      layer_manager_ = manager;
      for (auto& i : layer_) {
        layer_manager_->Set(i);
      }
    }
  private:

    LayerManager layer_manager_;
    Array<PageLayer<ImageLayer>> layer_;
  };
}
