#pragma once
#include <MmdNovel/kag_executor.h>
namespace kag {
  class FileExecutor : public Executor
  {
  public:
    FileExecutor();
    FileExecutor(const FilePath& filename);
    void Update();

    int NowLine()const;
  private:

    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;
  };
}
