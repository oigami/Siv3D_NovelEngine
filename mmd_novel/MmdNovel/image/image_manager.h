#pragma once
#include <MmdNovel/image/image_layer.h>
#include <MmdNovel/default_value.h>
namespace kag {
  class Imagemanager {
  public:
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
