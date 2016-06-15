#pragma once
#include <MmdNovel/layer.h>
#include <MmdNovel/kag_executor.h>
namespace kag
{
  namespace type_traits
  {
    template<class T> struct GetType
    {
      using type = T;
      using shared = std::shared_ptr<T>;
    };
    template<class T> struct GetType<std::shared_ptr<T>>
    {
      using type = T;
      using shared = std::shared_ptr<T>;
    };
  }

  struct IManager
  {
  public:

    IManager(const Executor& executor);

    virtual ~IManager() = default;

    PageLayer<LayerPtr>& GetLayer(int index);

    LayerPtr& GetLayer(int index, LayerPage page);

  protected:

    template<class Type> Type GetLayer(int index, LayerPage& page)
    {
      return Cast<Type>(GetLayer(index, page));
    }

    template<class Type> static typename type_traits::GetType<Type>::shared Cast(LayerPtr& layer)
    {
      using Ptr = typename type_traits::GetType<Type>::type;
      static_assert(std::is_base_of<Layer, Ptr>::value, "型はレイヤを継承している必要があります");
      static_assert(std::is_convertible<Ptr*, Layer*>::value, "型がpublic継承しているかを確認してください");

      return std::dynamic_pointer_cast<Ptr>(layer);
    }

    void resize(int num, const LayerPtr& fore, const LayerPtr& back);

    Executor executor_;

  private:

    Array<PageLayer<LayerPtr>> layer_;
  };

  using IManagerPtr = std::shared_ptr<IManager>;
}
