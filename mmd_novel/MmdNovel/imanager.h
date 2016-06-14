#pragma once
#include <MmdNovel/layer.h>
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

  class IManager
  {
    IManager() = default;

  public:

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
      static_assert(std::is_base_of<Layer, Ptr>::value, "Œ^‚ÍƒŒƒCƒ„‚ğŒp³‚µ‚Ä‚¢‚é•K—v‚ª‚ ‚è‚Ü‚·");
      return std::dynamic_pointer_cast<Ptr>(layer);
    }

    void resize(int num, PageLayer<LayerPtr>& val);

  private:

    Array<PageLayer<LayerPtr>> layer_;
  };

}
