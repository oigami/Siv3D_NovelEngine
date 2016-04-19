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
      if (cange_drawble_func_) cange_drawble_func_();
    }

    void SetDrawableList(Array<std::shared_ptr<Layer>>& list) {
      for (auto& i : layer_) {
        list.push_back(i[0]);
      }
    }

    ImageLayer& GetLayer(int layer, LayerPage page) { return layer_[layer][page]; }
    
    void ChangeDrawbleLayerCallBack(const std::function<void()>& list) {
      cange_drawble_func_ = list;
    }

  private:

    std::function<void()> cange_drawble_func_;
    Array<std::array<ImageLayer, 2>> layer_;
  };
}
