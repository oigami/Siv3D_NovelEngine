#pragma once
#include <MmdNovel/imanager.h>
#include <MmdNovel/mmd/mmd_layer.h>
#include <MmdNovel/kag_token.h>
namespace kag
{
  struct MMDManager : IManager, std::enable_shared_from_this<MMDManager>
  {
    void AddTag(FuncList& func_list) override;

  public:

    MMDManager(const Executor& exe);

    void resize(int num);

    MMDLayer GetLayer(int index, LayerPage page);

    struct MMDVal
    {
      MMDVal(CommandToken& token, const std::shared_ptr<MMDManager>& manager);
      MMDVal(const std::shared_ptr<MMDManager>& manager);

      Optional<SnapShotSpan> storage, vmd;
      LayerPage page = LayerPage::Fore;
      int start_time = 0;
      bool loop = true;
      bool visible = true;
      std::shared_ptr<MMDManager> manager_;
      void attach() const;
    };

    MMDVal getMMDVal(CommandToken& token);
    MMDVal getMMDVal();

    void MMDTag(CommandToken& token);

  };
  using MMDManagerPtr = std::shared_ptr<MMDManager>;
}
