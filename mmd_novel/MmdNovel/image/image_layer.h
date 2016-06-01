#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag {

  class ImageLayerPimpl : public Layer {
    friend LayerHelper<ImageLayerPimpl>;
  public:

    void SetTex(const Texture& tex) {
      texture_ = tex;
      SetPositionWidth(tex.width);
      SetPositionHeight(tex.height);
    }

  private:

    Texture texture_;

  protected:
    ImageLayerPimpl() = default;

    static std::shared_ptr<ImageLayerPimpl> create() {
      struct i :ImageLayerPimpl {};
      return std::make_shared<i>();
    }

    void draw() const {
      if (!texture_) return;
      position()(texture_).draw(Color(0xff, 0xff, 0xff, opacity()));
    }
  };
  using ImageLayer = LayerHelper<ImageLayerPimpl>;
}
