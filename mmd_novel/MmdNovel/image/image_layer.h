#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag {

  class ImageLayerPimpl : public Layer
  {
    friend LayerHelper<ImageLayerPimpl>;
  public:

    void SetTex(const Texture& tex);

  private:

    Texture texture_;

  protected:
    ImageLayerPimpl() = default;

    static std::shared_ptr<ImageLayerPimpl> create();

    void draw() const;
  };
  using ImageLayer = LayerHelper<ImageLayerPimpl>;
}
