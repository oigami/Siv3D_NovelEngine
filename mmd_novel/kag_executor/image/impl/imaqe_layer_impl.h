#pragma once
#include <MmdNovel/image/image_layer.h>
namespace kag {
  class ImageLayer::Pimpl : public Layer {
    Texture texture_;
  public:
    void draw() const {
      //TODO
      //if (!visible_) return;
      if (!texture_) return;
      position_(texture_).draw(Color(0xff, 0xff, 0xff, opacity_));
    }

  public:

    void SetTex(const Texture& tex) {
      texture_ = tex;
      position_.w = tex.width;
      position_.h = tex.height;
    }
  };
}
