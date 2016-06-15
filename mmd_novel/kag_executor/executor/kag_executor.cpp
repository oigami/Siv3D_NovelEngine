#include <MmdNovel/kag_executor.h>
#include <MmdNovel/tag_editor.h>
#include <kag_executor/executor/impl/kag_executor_impl.h>
#include <MmdNovel/imanager.h>
namespace kag
{
  Executor::Executor() :pimpl_(std::make_shared<Pimpl>()) {}

  PageLayer<LayerPtr> Executor::GetLayer(std::pair<kag::converter::LayerType, int> layer_num) const
  {
    using namespace converter;
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

  bool Executor::IsWait() const
  {
    return true;
  }
  bool Executor::Update()
  {
    return false;
  }

  bool Executor::CommandUpdate()
  {
    if ( IsWait() )return false;
    return false;
  }

  void Executor::Draw()
  {
  }


}
