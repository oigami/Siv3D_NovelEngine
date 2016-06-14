#include "impl/kag_file_executor_impl.h"
namespace kag
{
  FileExecutor::FileExecutor()
  {
    pimpl_ = std::make_shared<Pimpl>(*this);
  }
  FileExecutor::FileExecutor(const FilePath & filename) :FileExecutor()
  {
    pimpl_->Load(filename);
  }

  void FileExecutor::Update()
  {
    pimpl_->Update();
  }
  int FileExecutor::NowLine()const
  {
    return pimpl_->NowFileLine();
  }

  void FileExecutor::AddManager(const SnapShotSpan & name, const std::shared_ptr<IManager>& manager)
  {
    Executor::AddManager(name, manager);
    manager->AddTag(pimpl_->tag_func_);
  }

}
