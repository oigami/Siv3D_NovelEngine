#pragma once
#include <memory>
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
}
