#include <MmdNovel/kag_executor.h>
#include <kag_executor/executor/impl/kag_executor_impl.h>
#include <MmdNovel/imanager.h>
namespace kag
{
  Executor::Executor() :pimpl_(std::make_shared<Pimpl>()) {}

  PageLayer& Executor::GetLayer(std::pair<LayerType, int> layer_num) const
  {
    auto it = manager_.find(layer_num.first);
    if ( it == manager_.end() )
      throw std::invalid_argument("Executor::GetLayer()");
    return it->second->GetLayer(layer_num.second);
  }

  LayerManager Executor::layerManager() const
  {
    return pimpl_->layerManager();
  }

  void Executor::Command(const std::function<void()>& f)
  {
    pimpl_->Command(f);
  }

  void Executor::AddManager(const SnapShotSpan & name, const IManagerPtr & manager)
  {
    manager_[name] = manager;
  }

  void Executor::ShowErrorMsg(const String & str) const
  {
    MessageBox::Show(str);
  }

  bool Executor::IsWait()
  {
    is_wait_ = false;
    for ( auto& i : manager_ )
    {
      is_wait_ |= i.second->IsWait();
    }
    return is_wait_;
  }


  bool Executor::Update()
  {
    is_wait_ = false;
    for ( auto& i : manager_ )
    {
      i.second->Update();
      is_wait_ |= i.second->IsWait();
    }
    layerManager()->Update();
    return !is_wait_;
  }

  bool Executor::CommandUpdate()
  {
    if ( IsWait() )return false;
    pimpl_->CommandUpdate();
    return true;
  }

  void Executor::Draw()
  {
    layerManager()->Draw();
  }


}
