#pragma once
#include <MmdNovel/layer.h>
#include <MMD.h>
namespace kag
{
  namespace pimpl
  {
    class MMDLayerPimpl : public  Layer
    {
      friend LayerHelper<MMDLayerPimpl>;
    public:
      MMDLayerPimpl() = default;
      static std::shared_ptr<MMDLayerPimpl> create()
      {
        return std::make_shared<MMDLayerPimpl>();
      }

      void SetModel(s3d_mmd::MMD model);

      void SetVMD(s3d_mmd::VMD vmd);

      void update();

      void draw() const;

      void IsLoop(bool loop);

      void SetTime(int time);

    private:

      s3d_mmd::VMD vmd_;
      s3d_mmd::MMD model_;

    };
  }

  using MMDLayer = std::shared_ptr<pimpl::MMDLayerPimpl>;
}
