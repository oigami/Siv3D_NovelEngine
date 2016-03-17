#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
namespace kag {
  enum class KAGTokenType {
    EndOfStream = -1,            /// <summary>  </summary>
    Unkown = 0,                  /// <summary> unkown </summary>
    SymbolNewLine,               /// <summary> \n </summary>
    Identifier,                  /// <summary> 識別子 </summary>

    Text,                        /// <summary> テキスト </summary>
    Comment,                     /// <summary> ; </summary>
    SymbolAsterisk,              /// <summary> * </summary>
    SymbolVerticalLine,          /// <summary> | </summary>

    SymbolOpenCommand,           /// <summary> [, @  </summary>
    SymbolCloseCommand,          /// <summary> ], \n </summary>
    SymbolAtMark,                /// <summary> @ </summary>
    SymbolEqual,                 /// <summary> = </summary>
    LiteralString,               /// <summary> "a" </summary>
    SymbolTab,                   /// <summary> \t </summary>
    END,
  };

  class SnapShotSpan {
  public:
    using string = String;
    using Char = string::value_type;
    SnapShotSpan() {}
    SnapShotSpan(const std::shared_ptr<string> &str, int s, int e)
      :str_(str), start_(s), end_(e), c_(str->data() + s) {
    }

    SnapShotSpan(const Char *str, int start, int end)
      :c_(str), start_(start), end_(end) {
    }

    SnapShotSpan(const String& str) :c_(str.data()), start_(0), end_(str.length) {

    }
    template<size_t n> SnapShotSpan(const Char(&str)[n])
      : c_(str), start_(0), end_(n - 1) {
    }



    /// <summary>
    /// 文字列を返す 末尾に\0がないので注意（lengthで長さを判定）
    /// </summary>
    /// <returns></returns>
    const Char *Str() const { return c_; }
    string ToStr() const { return string(c_, Length()); }
    std::string ToNarrow() const { return Narrow(StringView(c_, Length())); }

    Optional<int> ToInt() const {
      Char *e;
      int ret = wcstol(c_, &e, 10);
      if (e != c_ + Length())
        return none;
      return ret;
    }

    int Length() const { return end_ - start_; }
    int End() const { return end_; }
    int Start() const { return start_; }

    bool TextEqual(const SnapShotSpan& span) const {
      if (span.Length() != Length()) return false;
      return StrCmp(c_, span.c_, Length()) == 0;
    }
    bool operator<(const SnapShotSpan& r) const {
      return StrCmp(c_, r.c_, Length()) < 0;
    }

    bool operator==(const SnapShotSpan& r) const {
      return TextEqual(r);
    }

    Char operator[](std::uint32_t index) {
      assert(Length() > index);
      return c_[index];
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
    for (int i = 0; i < span.Length(); i++) {
      os << span.Str()[i];
    }
    return os;
  }

  template<class T> Optional<T> ToCast(const SnapShotSpan& span);

  template<> inline Optional<int> ToCast<int>(const SnapShotSpan & span) {
    return span.ToInt();
  }

  template<> inline Optional<String> ToCast<String>(const SnapShotSpan & span) {
    return span.ToStr();
  }

  template<> inline Optional<SnapShotSpan> ToCast<SnapShotSpan>(const SnapShotSpan & span) {
    return span;
  }

  class Tokenizer {
  public:
    enum class ParseType {
      CommandAtmark,                /// <summary> @ </summary>
      CommandBrancket,              /// <summary> [ </summary>
      Label,                        /// <summary> * </summary>
      Label2,                       /// <summary> | </summary>
      Label3,                       /// <summary> |以降 </summary>
      Normal,
    };

    using Char = String::value_type;
    class Token {
    public:

      Token() : type_(KAGTokenType::EndOfStream) {}
      Token(SnapShotSpan span, KAGTokenType type) :span_(span), type_(type) {}
      KAGTokenType Type()const { return type_; }

      const SnapShotSpan &Span()const { return span_; }

      bool TextEqual(const Token &token) const {
        return span_.TextEqual(token.span_);
      }

      bool operator==(KAGTokenType type) const { return type_ == type; }
      bool operator!=(KAGTokenType type) const { return type_ != type; }
    private:
      KAGTokenType type_;
      SnapShotSpan span_;

    };

    Tokenizer();

    explicit Tokenizer(const String &str);

    explicit Tokenizer(String &&str);

    explicit Tokenizer(const std::shared_ptr<String> &not_null_str);

    Token Read();

    const Token& NextToken() const { return next_token_; }

  private:
    void Reset();

    void ReadCommand();

    void ReadLabel();

    void ReadLabelHeading();

    void ReadNormal(KAGTokenType type);

    KAGTokenType IsCommandSpecialSymbol(int pos);

    std::shared_ptr<String> str_;

    Token next_token_;

    ParseType now_tokenize_;

    /// <summary>
    /// 解析が終わったポジション
    /// </summary>
    int now_pos_;
  };

}