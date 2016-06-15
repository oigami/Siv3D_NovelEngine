#pragma once
#include <MmdNovel/kag_executor.h>
#include <MmdNovel/imanager.h>
namespace kag
{
  class FileExecutor : public Executor
  {
  public:
    FileExecutor();
    FileExecutor(const FilePath& filename);
    void Update();

    int NowLine()const;

    void AddManager(const SnapShotSpan& name, const std::shared_ptr<IManager>& manager);

    template<class Manager> void AddManager(const SnapShotSpan& name)
    {
      static_assert(std::is_base_of<IManager, Manager>::value, "ManagerはIManagerを継承している必要があります");
      static_assert(std::is_convertible<Manager*, IManager*>::value, "Managerがpublic継承しているか確認してください");
      AddManager(name, std::make_shared<Manager>(*this));
    }

  private:

    class Pimpl;
    std::shared_ptr<Pimpl> pimpl_;
  };
}
