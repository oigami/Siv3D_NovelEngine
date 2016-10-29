#pragma once
#include <MmdNovel/layer/ilayer.h>
#include <MMD/MMD.h>
namespace kag
{
  namespace pimpl
  {
    class MMDLayerPimpl : public ILayer
    {
    public:
      void clone(ILayerPtr& out)override;

      MMDLayerPimpl(const LayerManager& manager);

      void SetModel(s3d_mmd::MMD model);

      void SetVMD(s3d_mmd::VMD vmd);

      void update();

      void draw() const;

      void IsLoop(bool loop);

      void SetTime(int time);

      void setPos(const Point& pos)
      {
        pos_ = pos;
      }
      physics3d::Physics3DWorld world;

    private:

      s3d_mmd::VMD vmd_;
      s3d_mmd::MMD model_;
      Point pos_ = { 0,0 };
    };
  }

  using MMDLayer = std::shared_ptr<pimpl::MMDLayerPimpl>;
}
