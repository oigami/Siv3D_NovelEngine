#include "image_layer.h"
#include <kag_executor/image/impl/imaqe_layer_impl.h>
namespace kag {
  ImageLayer::operator std::shared_ptr<Layer>() const {
    return pimpl_;
  }

  ImageLayer::ImageLayer() {
    pimpl_ = std::make_shared<Pimpl>();
  }

  void ImageLayer::SetTex(const Texture& tex) {
    pimpl_->SetTex(tex);
  }

  void ImageLayer::SetZIndex(uint16 z) {
    pimpl_->SetZIndex(z);
  }

}
