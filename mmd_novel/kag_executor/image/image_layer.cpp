#include <MmdNovel/image/image_layer.h>

namespace kag
{
  namespace pimpl
  {
    void ImageLayerPimpl::clone(LayerPtr & out)
    {
      DefaultClone(*this, out);
    }

    void ImageLayerPimpl::SetTex(const Texture & tex)
    {
      texture_ = tex;
      SetPositionWidth(tex.width);
      SetPositionHeight(tex.height);
    }

    void ImageLayerPimpl::draw() const
    {
      if ( !texture_ ) return;
      position()(texture_).draw(Color(0xff, 0xff, 0xff, opacity()));
    }
  }
}
