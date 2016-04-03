#include "impl/kag_file_executor_impl.h"
namespace kag {
  FileExecutor::FileExecutor() {
    pimpl_ = std::make_shared<Pimpl>(*this);
  }
  FileExecutor::FileExecutor(const FilePath & filename) :FileExecutor() {
    pimpl_->Load(filename);
  }

  void FileExecutor::Update() {
    pimpl_->Update();
  }

}
