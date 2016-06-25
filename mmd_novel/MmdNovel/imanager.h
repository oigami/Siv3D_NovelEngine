#pragma once
#include <MmdNovel/kag_executor.h>
#include <MmdNovel/type_traits.h>
namespace kag
{
  struct ILayer;
  using ILayerPtr = std::shared_ptr<ILayer>;

  struct IManager
  {
  private:
    virtual void update() = 0;
  public:
    virtual bool IsWait() const { return false; }

    IManager(const std::weak_ptr<Executor>& executor);

    virtual ~IManager();

    PageLayer& GetLayer(int index);

    ILayerPtr& GetLayer(int index, LayerPage page);

    int size() const;

    void Update();

  protected:

    template<class Type> type_traits::GetType_s<Type> GetLayer(int index, LayerPage& page)
    {
      return Cast<Type>(GetLayer(index, page));
    }

    template<class Type> static type_traits::GetType_s<Type> Cast(ILayerPtr& layer)
    {
      using Ptr = type_traits::GetType_t<Type>;
      static_assert(std::is_base_of<ILayer, Ptr>::value, "型はレイヤを継承している必要があります");
      static_assert(std::is_convertible<Ptr*, ILayer*>::value, "型がpublic継承しているかを確認してください");

      auto res = std::dynamic_pointer_cast<Ptr>(layer);
      assert(!!res);
      return res;
    }

    void resize(int num, const ILayerPtr& fore, const ILayerPtr& back);

    std::shared_ptr<Executor> GetExecutor();

  private:
    std::weak_ptr<Executor> executor_;

    LayerManager layer_manager_;
    Array<PageLayer> layer_;
  };

  using IManagerPtr = std::shared_ptr<IManager>;
}
