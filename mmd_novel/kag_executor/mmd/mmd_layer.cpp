#include <MmdNovel/mmd/mmd_layer.h>
namespace kag
{
  namespace pimpl
  {
    void MMDLayerPimpl::clone(ILayerPtr & out) { return DefaultClone(*this, out); }

    MMDLayerPimpl::MMDLayerPimpl(const LayerManager & manager) :ILayer(manager) {}

    void MMDLayerPimpl::SetModel(s3d_mmd::MMD model)
    {
      model_ = model;
      model_.attach(vmd_);
    }

    void MMDLayerPimpl::SetVMD(s3d_mmd::VMD vmd)
    {
      vmd_ = vmd;
      vmd_.SetPosFrame(0);
      vmd_.play();
      if ( model_.isOpen() ) model_.attach(vmd_);
    }
    void MMDLayerPimpl::update()
    {
      world.update();
      if ( model_.isOpen() )
      {
        model_.update();
      }
    }
    void MMDLayerPimpl::draw() const
    {
      if ( model_.isOpen() )
      {
        const static Mesh meshGround(MeshData::Plane({ 40, 40 }, { 6, 6 }));
        //meshGround.draw();

        model_.draw(Mat4x4::Translate(pos_.x, pos_.y, 0.0f));
      }
    }
    void MMDLayerPimpl::IsLoop(bool loop)
    {
      vmd_.setLoop(loop);
    }
    void MMDLayerPimpl::SetTime(int time)
    {
      vmd_.SetPosFrame(time);
    }
  }
}
