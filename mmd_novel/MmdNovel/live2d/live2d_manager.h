#pragma once
#include <MmdNovel/kag_file_executor.h>
#include <MmdNovel/live2d/live2d_layer.h>
#include <lib/Siv3D_Live2D/include/siv3d_live2d.h>
namespace kag
{
  namespace live2d
  {
    class Live2DManager : public kag::file::IFileManagerType<Live2DManager>
    {
      virtual void AddTag(FuncList& list) override;
      virtual void update() override;
      Live2D live2d_;
    public:

      Live2DManager(const std::weak_ptr<Executor>& exe);

      void resize(int num);

      void Live2DTag(kag::file::CommandToken& token);

    };
  }
}
