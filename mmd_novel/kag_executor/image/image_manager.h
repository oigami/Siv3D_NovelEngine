#pragma once
#include <kag_executor/image/image_layer.h>
#include <MmdNovel/default_value.h>
namespace kag {
  class Imagemanager {
  public:
    void Draw() {
      for (auto& i : layer_) {
        i[Define::fore_page].Draw();
      }
    }

    void SetTex(Texture &tex, int layer, Value<int> page) {
      layer_[layer][page(Define::fore_page)].SetTex(tex);
    }

  private:
    Array<std::array<ImageLayer, 2>> layer_;
  };
}
