#include <MmdNovel/mmd/mmd_manager.h>
#include <MmdNovel/kag_executor.h>
#define GET(name) get(L#name,name)
namespace kag
{

  void MMDManager::AddTag(FuncList & func_list)
  {
    func_list[L"mmd"] = [this_ = shared_from_this()](CommandToken& token) { this_->MMDTag(token); };
  }

  MMDManager::MMDManager(const Executor & exe) :IManager(exe)
  {
    resize(1);

  }

  void MMDManager::resize(int num)
  {
    static const PageLayer<MMDLayer> default_layer = {
      std::make_shared<pimpl::MMDLayerPimpl>(),
      std::make_shared<pimpl::MMDLayerPimpl>()
    };

    IManager::resize(num, default_layer.Fore(), default_layer.Back());
  }

  MMDLayer MMDManager::GetLayer(int index, LayerPage page)
  {
    return IManager::GetLayer<MMDLayer>(index, page);
  }

  MMDManager::MMDVal MMDManager::getMMDVal(CommandToken & token)
  {
    return MMDVal(token, this->shared_from_this());
  }

  MMDManager::MMDVal MMDManager::getMMDVal()
  {
    return MMDVal(this->shared_from_this());
  }

  void MMDManager::MMDTag(CommandToken & token)
  {
    MMDVal tag(token, this->shared_from_this());
    if ( token.HasError() ) return;
    executor_.Command([tag = std::move(tag)]() { tag.attach(); });
  }

  MMDManager::MMDVal::MMDVal(CommandToken & token, const MMDManagerPtr & manager) : manager_(manager)
  {
    token.GET(storage).GET(vmd).GET(loop).GET(start_time).GET(visible).GET(page);
  }

  MMDManager::MMDVal::MMDVal(const MMDManagerPtr & manager) : manager_(manager)
  {
  }

  void MMDManager::MMDVal::attach() const
  {
    auto layer = manager_->GetLayer(0, page);
    layer->IsVisible(visible);
    if ( storage ) layer->SetModel(storage->ToStr());
    if ( vmd ) layer->SetVMD(vmd->ToStr());
    layer->IsLoop(loop);
    layer->SetTime(start_time);
  }
}
