#include "mmd_layer.h"
namespace kag {
  namespace pimpl {

    void MMDLayerPimpl::SetModel(s3d_mmd::MMD model) {
      model_ = model;
    }
    void MMDLayerPimpl::SetVMD(s3d_mmd::VMD vmd) {
      vmd_ = vmd;
      vmd_.setTime(0);
    }
    void MMDLayerPimpl::update() {
      if (model_.isOpen()) {
        vmd_.UpdateTime();
        vmd_.UpdateBone(*model_.bones());
      }
    }
    void MMDLayerPimpl::draw() const {
      if (model_.isOpen()) {
        model_.draw(vmd_);
      }
    }
    void MMDLayerPimpl::IsLoop(bool loop) {
      vmd_.IsLoop(loop, 0);
    }
    void MMDLayerPimpl::SetTime(int time) {
      vmd_.setTime(time);
    }
  }
}
