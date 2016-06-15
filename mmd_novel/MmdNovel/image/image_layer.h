#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer.h>
namespace kag
{
  namespace pimpl
  {
    class ImageLayerPimpl : public Layer
    {
    public:
      using Layer::Layer;

      void SetTex(const Texture& tex);

    private:

      Texture texture_;

    protected:

      void draw() const;
    };
  }
  using ImageLayer = std::shared_ptr<pimpl::ImageLayerPimpl>;
}
