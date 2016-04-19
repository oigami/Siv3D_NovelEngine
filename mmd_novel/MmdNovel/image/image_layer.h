#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag {

  class ImageLayer {
  public:
    ImageLayer();
    operator std::shared_ptr<Layer>() const;

    void SetTex(const Texture& tex);

    void SetZIndex(uint16 z);

  private:

    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;
  };
}
