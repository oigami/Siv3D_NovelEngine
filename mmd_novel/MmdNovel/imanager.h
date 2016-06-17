﻿#pragma once
#include <MmdNovel/layer.h>
#include <MmdNovel/kag_executor.h>
#include <MmdNovel/type_traits.h>
namespace kag
{

  struct IManager
  {
  private:
    virtual void update() = 0;
  public:
    virtual bool IsWait() const { return false; }

    IManager(const Executor& executor);

    virtual ~IManager() = default;

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
      static_assert(std::is_base_of<Layer, Ptr>::value, "型はレイヤを継承している必要があります");
      static_assert(std::is_convertible<Ptr*, Layer*>::value, "型がpublic継承しているかを確認してください");

      auto res = std::dynamic_pointer_cast<Ptr>(layer);
      assert(!!res);
      return res;
    }

    template<class Layer> void resize(int num, const Layer& fore, const Layer& back)
    {
      using type = typename type_traits::GetType<Layer>::type;
      const int pre_size = size();
      for ( int i = num; i < pre_size; i++ )
      {
        executor_.layerManager()->Remove(layer_[i]);
      }
      layer_.reserve(num);
      for ( int i = pre_size; i < num; i++ )
      {
        layer_.push_back(PageLayer<LayerPtr>{ std::make_shared<type>(*fore), std::make_shared<type>(*back) });
        executor_.layerManager()->Set(layer_[i]);
      }
    }

    void resize(int num, const LayerPtr& fore, const LayerPtr& back);

    Executor executor_;

  private:

    Array<PageLayer<LayerPtr>> layer_;
  };

  using IManagerPtr = std::shared_ptr<IManager>;
}
