#pragma once
namespace kag {
  namespace detail {
    struct Default {
      constexpr operator int() const { return -1; }
    };
    template<class Type> constexpr bool operator==(const Type& t, const Default&) {
      return t == static_cast<Type>(Define::default);
    }
  }
  /// <summary>
  /// kagで使用する各種定数値
  /// </summary>
  namespace Define {
    constexpr int fore_page = 0;
    constexpr int back_page = 1;
    constexpr detail::Default default;
  }

  template<class Type> class Value {
  public:
    constexpr Value(const Type& v) :t(v) {}
    constexpr Value() : t(Define::default) {}
    constexpr operator const Type&() const { return t; }
    Type& operator=(const Type& v) {
      t = v;
      return t;
    }
    constexpr bool operator==(const detail::Default&) const {
      return t == Define::default;
    }
    /// <summary>
    /// 現在の値がデフォルト値の場合は引数を返す
    /// </summary>
    /// <param name="def"></param>
    /// <returns></returns>
    Type operator()(Type def) const { return t == Define::default ? def : t; }
  private:

    Type t;
  };

}
