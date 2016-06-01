#include <MmdNovel/image/image_layer.h>

namespace kag {
  void ImageLayerPimpl::SetTex(const Texture & tex) {
    texture_ = tex;
    SetPositionWidth(tex.width);
    SetPositionHeight(tex.height);
  }
  std::shared_ptr<ImageLayerPimpl> ImageLayerPimpl::create() {
    struct i :ImageLayerPimpl {};
    return std::make_shared<i>();
  }
  void ImageLayerPimpl::draw() const {
    if (!texture_) return;
    position()(texture_).draw(Color(0xff, 0xff, 0xff, opacity()));
  }
}
