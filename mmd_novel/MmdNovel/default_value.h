#pragma once
namespace kag {
  namespace detail {
    struct Default {
      constexpr operator int() const { return std::numeric_limits<int>::min() + 10000000; }
      template<class Type> constexpr bool operator==(const Type& t) const {
        return t == static_cast<Type>(Define::default);
      }
    };
    template<class Type> constexpr bool operator==(const Type& t, const Default&) {
      return Define::default == t;
    }
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
    constexpr Type operator()(Type def) const { return t == Define::default ? def : t; }
  private:

    Type t;
  };

  /// <summary>
  /// kagで使用する各種定数値
  /// </summary>
  namespace Define {

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

}
