#include <MmdNovel/mmd/mmd_manager.h>
#include <MmdNovel/kag_executor.h>
#define GET(name) get(L#name,name)
namespace kag
{
  namespace file
  {
    void MMDManager::AddTag(FuncList & func_list)
    {
      func_list[L"mmd"] = Bind(&MMDManager::MMDTag);
      func_list[L"camera"] = Bind(&MMDManager::CameraTag);
    }

    MMDManager::MMDManager(const std::weak_ptr<Executor>& exe) :IFileManagerType(exe)
    {
      resize(1);

    }

    void MMDManager::resize(int num)
    {
      static const PageLayer default_layer = {
        GetExecutor()->MakeLayer<MMDLayer>(),
        GetExecutor()->MakeLayer<MMDLayer>(),
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
      GetExecutor()->Command([tag = std::move(tag)]() { tag.attach(); });
    }

    MMDManager::MMDVal::MMDVal(CommandToken & token, const MMDManagerPtr & manager) : manager_(manager)
    {
      token.GET(storage).GET(vmd).GET(loop).GET(start_time).GET(visible).GET(page).GET(x).GET(y);
    }

    MMDManager::MMDVal::MMDVal(const MMDManagerPtr & manager) : manager_(manager)
    {
    }

    void MMDManager::MMDVal::attach() const
    {
      auto layer = manager_->GetLayer(0, page);
      layer->IsVisible(visible);
      if ( storage ) layer->SetModel({ storage->ToStr(),layer->world });
      if ( vmd ) layer->SetVMD(vmd->ToStr());
      layer->IsLoop(loop);
      layer->SetTime(start_time);
      layer->setPos({ x,y });
    }

    void MMDManager::CameraTag(CommandToken & token)
    {
      struct CameraVal
      {
        Optional<Vec3> pos_, lookat_;
        CameraVal(CommandToken& token)
        {
          Optional<SnapShotSpan> pos, lookat;
          token.GET(pos).GET(lookat);
          Vec3 tmp;
          if ( pos && swscanf(pos->Str(), L"(%lf,%lf,%lf)", &tmp.x, &tmp.y, &tmp.z) == 3 )
            pos_ = tmp;
          if ( lookat && swscanf(lookat->Str(), L"(%lf,%lf,%lf)", &tmp.x, &tmp.y, &tmp.z) == 3 )
          {
            lookat_ = tmp;
          }
        }
        void attach() const
        {
          Camera camera;
          if ( pos_ ) camera.pos = *pos_;
          if ( lookat_ ) camera.lookat = *lookat_;
          Graphics3D::SetCamera(camera);
        }
      };
      GetExecutor()->Command([tag = CameraVal(token)](){ tag.attach(); });
    }
  }
}
