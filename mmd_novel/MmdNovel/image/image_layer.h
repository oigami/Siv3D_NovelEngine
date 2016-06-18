#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/layer/ilayer.h>
namespace kag
{
  namespace pimpl
  {
    class ImageLayerPimpl : public ILayer
    {
      virtual void clone(LayerPtr& out);

    public:
      using ILayer::ILayer;

      void SetTex(const Texture& tex);

    private:

      Texture texture_;

    protected:

      void draw() const;
    };
  }
  using ImageLayer = std::shared_ptr<pimpl::ImageLayerPimpl>;
}
