#pragma once
#include <MmdNovel/layer.h>
#include <MMD.h>
namespace kag
{
  namespace pimpl
  {
    class MMDLayerPimpl : public ILayer
    {
    public:
      MMDLayerPimpl(const LayerManager& manager);

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
