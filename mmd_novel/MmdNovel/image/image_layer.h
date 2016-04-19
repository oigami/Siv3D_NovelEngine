#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag {

  class ImageLayer : public Layer {
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

  private:

    Texture texture_;
  };
}
