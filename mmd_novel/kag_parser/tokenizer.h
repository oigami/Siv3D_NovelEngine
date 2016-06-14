#pragma once
#include <Siv3D.hpp>
#include <unordered_map>
#include <MmdNovel/message/snapshotspan.h>
namespace kag
{
  enum class KAGTokenType
  {
    EndOfStream = -1,            /// <summary>  </summary>
    Unkown = 0,                  /// <summary> unkown </summary>
    SymbolNewLine,               /// <summary> \n </summary>
    Identifier,                  /// <summary> 識別子 </summary>

    EscapeText,
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


  class Tokenizer
  {
  public:
    enum class ParseType
    {
      CommandAtmark,                /// <summary> @ </summary>
      CommandBrancket,              /// <summary> [ </summary>
      Label,                        /// <summary> * </summary>
      Label2,                       /// <summary> | </summary>
      Label3,                       /// <summary> |以降 </summary>
      Normal,
    };

    using Char = String::value_type;
    class Token
    {
    public:

      Token() : type_(KAGTokenType::EndOfStream) {}
      Token(const SnapShotSpan& span, KAGTokenType type) :span_(span), type_(type) {}
      KAGTokenType Type()const { return type_; }

      const SnapShotSpan &Span()const { return span_; }

      bool TextEqual(const Token &token) const
      {
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

    int now_line_;
  };

}
