#pragma once
#include <MmdNovel/layer/ilayer.h>
#include <lib/Siv3D_Live2D/include/siv3d_live2d.h>

namespace kag
{
  namespace live2d
  {
    struct Live2DLayerPimpl : public ILayer
    {
    public:
      Live2DModel model;

      void clone(ILayerPtr& out)override;

      Live2DLayerPimpl(const LayerManager& manager) : ILayer(manager)
      {
        model.model = nullptr;
      }

      void draw() const override
      {
        if ( model.model )
          Live2D::AddDrawObject(model);
      }
    };
  }
}
