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
    void Draw() {
      for (auto& i : layer_) {
        i[Define::fore_page].Draw();
      }
    }

    void SetTex(const Texture &tex, int layer, Value<LayerPage> page) {
      layer_[layer][page(LayerPage::Fore)].SetTex(tex);
    }

  private:
    Array<std::array<ImageLayer, 2>> layer_;
  };
}
