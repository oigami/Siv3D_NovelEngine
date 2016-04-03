#pragma once
namespace kag {
  struct Default {
    constexpr operator int() const { return -1; }
  };
  constexpr Default default;

  template<class Type> class Value {
  public:
    constexpr Value(const Type& v) :t(v) {}
    constexpr Value() : t(kag::default) {}
    constexpr operator const Type&() const { return t; }
    Type& operator=(const Type& v) {
      t = v;
      return t;
    }
    constexpr bool operator==(const Default& def) const {
      return t == def;
    }
  private:

    Type t;
  };
  template<class Type> constexpr bool operator==(const Type& t, const Default&) {
    return t == static_cast<Type>(kag::default);
  }
}
