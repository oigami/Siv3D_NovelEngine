#pragma once
#include <limits>
namespace kag
{
  template<class Type> class Value;
  namespace detail
  {
    struct Default
    {
      constexpr Default() = default;
      constexpr operator int() const { return std::numeric_limits<int>::min() / 4; }
      template<class Type> constexpr bool operator==(const Type& t) const
      {
        return t == static_cast<Type>(Define::default);
      }
    };
    template<class Type> constexpr bool operator==(const Value<Type>& t, const Default&)
    {
      return Define::default == t;
    }
  }

  /// <summary>
  /// kagで使用する各種定数値
  /// </summary>
  namespace Define
  {
    // 背景レイヤのインデックス値
    // 前景レイヤは1から始める
    constexpr int base_image = 0;

    // 表ページ
    constexpr int fore_page = 0;

    // 裏ページ
    constexpr int back_page = 1;

    //省略した時の値
    constexpr kag::detail::Default default;
  }

  template<class Type> class Value
  {
  public:
    constexpr Value(const Type& v) :t(v) {}
    constexpr Value() : t(detail::Default()) {}

    Type& operator=(const Type& v)
    {
      t = v;
      return t;
    }
    constexpr bool operator==(const detail::Default&) const
    {
      return t == Value().t;
    }

    Type& operator()() { return t; }
    constexpr const Type& operator()() const { return t; }

    Type& operator*() { return t; }
    constexpr const Type& operator*() const { return t; }

    constexpr const Type& get_or_throw() const
    {
      if ( t == Define::default )
        throw std::runtime_error("Value is empty");
      return t;
    }

    /// <summary>
    /// 現在の値がデフォルト値の場合は引数を返す
    /// </summary>
    /// <param name="def"></param>
    /// <returns></returns>
    constexpr Type operator()(Type def) const { return t == Define::default ? def : t; }
  private:

    Type t;
  };


}
