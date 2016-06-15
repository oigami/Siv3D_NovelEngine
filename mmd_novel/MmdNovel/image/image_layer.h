#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag
{
  namespace pimpl
  {
    class ImageLayerPimpl : public Layer
    {
      friend LayerHelper<ImageLayerPimpl>;
    public:
      ImageLayerPimpl() = default;

      void SetTex(const Texture& tex);

    private:

      Texture texture_;

    protected:

      static std::shared_ptr<ImageLayerPimpl> create();

      void draw() const;
    };
  }
  using ImageLayer = std::shared_ptr<pimpl::ImageLayerPimpl>;
}
