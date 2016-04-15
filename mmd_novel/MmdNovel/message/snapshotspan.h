#pragma once
#include <Siv3D.hpp>
namespace kag {

  class SnapShotSpan {
  public:
    using string = String;
    using Char = string::value_type;
  private:
    explicit SnapShotSpan(const Char* c, int start, int end) : c_(c), start_(start), end_(end) {
    }
  public:
    SnapShotSpan() {}
    SnapShotSpan(const std::shared_ptr<string> &str, int s, int e)
      :str_(str), start_(s), end_(e), c_(str->data() + s) {
    }

    template<size_t n> SnapShotSpan(const Char(&str)[n], int start, int end)
      : c_(str), start_(start), end_(end) {
    }

    template<size_t n> SnapShotSpan(const Char(&str)[n])
      : c_(str), start_(0), end_(n - 1) {
    }


    SnapShotSpan substr(std::uint_fast32_t start, std::uint_fast32_t count) const {
      int new_start = start_ + start;
      int new_end = new_start + count;
      assert(new_end <= end_);
      if (str_) {
        return SnapShotSpan(str_, new_start, new_end);
      } else {
        return SnapShotSpan(c_ + start, new_start, new_end);
      }
    }
    /// <summary>
    /// 文字列を返す 末尾に\0がないので注意（lengthで長さを判定）
    /// </summary>
    /// <returns></returns>
    const Char *Str() const { return c_; }
    string ToStr() const { return string(c_, Length()); }
    std::string ToNarrow() const { return Narrow(StringView(c_, Length())); }

    int Length() const { return end_ - start_; }
    int End() const { return end_; }
    int Start() const { return start_; }

    bool TextEqual(const SnapShotSpan& span) const {
      if (span.Length() != Length()) return false;
      return StrCmp(c_, span.c_, Length()) == 0;
    }
    bool operator<(const SnapShotSpan& r) const {
      const int len = Length();
      const int len2 = r.Length();
      const int a = StrCmp(c_, r.c_, std::min(len, len2));
      if (a == 0)return len < len2;
      if (a < 0) return true;
      return false;
    }

    bool operator==(const SnapShotSpan& r) const {
      return TextEqual(r);
    }

    bool operator!=(const SnapShotSpan& r) const {
      return !(*this == r);
    }

    const Char& operator[](std::int32_t index) const {
      assert(0 <= index && index < Length());
      return c_[index];
    }

    size_t hash() const {
      return std::_Hash_seq(reinterpret_cast<const unsigned char*>(c_), Length() * sizeof(Char));
    }

  private:

    static int StrCmp(const char* l, const char* r, size_t size) {
      return ::strncmp(l, r, size);
    }

    static int StrCmp(const wchar_t* l, const wchar_t*  r, size_t size) {
      return ::wcsncmp(l, r, size);
    }
    const Char *c_;
    int start_;
    int end_;
    std::shared_ptr<string> str_;
  };

  inline std::wostream &operator<<(std::wostream &os, const SnapShotSpan &span) {
    const auto& str = span.Str();
    for (int i = 0; i < span.Length(); i++) {
      os << str[i];
    }
    return os;
  }
}