#pragma once
#include <MmdNovel/kag_executor.h>
#include <MmdNovel/type_traits.h>
namespace kag
{
  struct ILayer;
  using LayerPtr = std::shared_ptr<ILayer>;

  struct IManager
  {
  private:
    virtual void update() = 0;
  public:
    virtual bool IsWait() const { return false; }

    IManager(const std::weak_ptr<Executor>& executor);

    virtual ~IManager();

    PageLayer<LayerPtr>& GetLayer(int index);

    LayerPtr& GetLayer(int index, LayerPage page);

    int size() const;

    void Update();

  protected:

    template<class Type> typename type_traits::GetType<Type>::shared GetLayer(int index, LayerPage& page)
    {
      return Cast<Type>(GetLayer(index, page));
    }

    template<class Type> static typename type_traits::GetType<Type>::shared Cast(LayerPtr& layer)
    {
      using Ptr = typename type_traits::GetType<Type>::type;
      static_assert(std::is_base_of<ILayer, Ptr>::value, "型はレイヤを継承している必要があります");
      static_assert(std::is_convertible<Ptr*, ILayer*>::value, "型がpublic継承しているかを確認してください");

      auto res = std::dynamic_pointer_cast<Ptr>(layer);
      assert(!!res);
      return res;
    }

    template<class Layer> void resize(int num, const Layer& fore, const Layer& back)
    {
      using type = typename type_traits::GetType<Layer>::type;
      const int pre_size = size();
      auto manager = GetExecutor()->layerManager();
      for ( int i = num; i < pre_size; i++ )
      {
        manager->Remove(layer_[i]);
      }
      layer_.reserve(num);
      for ( int i = pre_size; i < num; i++ )
      {
        layer_.push_back(PageLayer<LayerPtr>{ std::make_shared<type>(*fore), std::make_shared<type>(*back) });
        manager->Set(layer_[i]);
      }
    }

    void resize(int num, const LayerPtr& fore, const LayerPtr& back);

    std::shared_ptr<Executor> GetExecutor();

  private:
    std::weak_ptr<Executor> executor_;

    LayerManager layer_manager_;
    Array<PageLayer<LayerPtr>> layer_;
  };

  using IManagerPtr = std::shared_ptr<IManager>;
}
