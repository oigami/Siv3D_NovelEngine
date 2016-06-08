#include<string>
#include <kag_parser/tokenizer.h>
namespace kag {

  Tokenizer::Tokenizer() {
  }

  Tokenizer::Tokenizer(const String& str) {
    str_ = std::make_shared<String>();
    str_->reserve(str.length + 1);
    *str_ += str;
    *str_ += L'\n';
    Reset();
  }

  Tokenizer::Tokenizer(String &&str) {
    if (!str.endsWith(L'\n')) {
      str.reserve(str.length + 1);
      str += L'\n';
    }
    str_ = std::make_shared<String>(std::move(str));
    Reset();
  }
  Tokenizer::Tokenizer(const std::shared_ptr<String> &str_p) {
    assert(str_p.get());
    auto& str = *str_p;
    if (!str.endsWith(L'\n')) {
      str.reserve(str.length + 1);
      str += L'\n';
    }
    str_ = str_p;
    Reset();
  }

  void Tokenizer::Reset() {
    now_pos_ = -1;
    now_tokenize_ = ParseType::Normal;
    ReadNormal(KAGTokenType::SymbolNewLine);
    if (next_token_ == KAGTokenType::SymbolNewLine) Read();
  }

  bool IsSpace(char c) {
    return isspace(c) != 0;
  }
  bool IsSpace(wchar_t c) {
    return iswspace(c) != 0;
  }

  bool IsTab(char c) {
    return c == '\t';
  }
  bool IsTab(wchar_t c) {
    return c == L'\t';
  }

  KAGTokenType IsNewLineSpecialSymbol(const String& c, int pos) {
    switch (c[pos]) {
    case L'\t': return KAGTokenType::SymbolTab;
    case L'@':  return KAGTokenType::SymbolAtMark;
    case L';':  return KAGTokenType::Comment;
    case L'*':  return KAGTokenType::SymbolAsterisk;
    default:    return KAGTokenType::SymbolNewLine;
    }
  }

  /// <summary>
  /// 通常時の特殊文字華道家の判定をする
  /// ']' は '[' がきてる時のみ特殊文字として判定するのでここでは判定しないでtextとして返す
  /// </summary>
  /// <param name="c"></param>
  /// <param name="pos"></param>
  /// <returns></returns>
  KAGTokenType IsNormalSpecialSymbol(const String& c, int& pos) {
    switch (c[pos]) {
    case L'[':
      if (c[pos + 1] != L'[') // [[の場合は普通の文字になるのでチェック
        return KAGTokenType::SymbolOpenCommand;
      pos++;
      return KAGTokenType::EscapeText;
      break;
    case L'\n':
      return KAGTokenType::SymbolNewLine;
    }
    return KAGTokenType::Text;
  }

  Tokenizer::Token Tokenizer::Read() {
    Token token = std::move(next_token_);
    int size = static_cast<int>(str_->length);
    if (now_pos_ >= size) {
      next_token_ = Token({ str_ , size ,size }, KAGTokenType::EndOfStream);
      return token;
    }
    switch (now_tokenize_) {
    case ParseType::CommandAtmark:
    case ParseType::CommandBrancket:
      ReadCommand();
      break;
    case ParseType::Normal:
      ReadNormal(IsNormalSpecialSymbol(*str_, now_pos_));
      break;
    case ParseType::Label:
      ReadLabel();
      break;
    case ParseType::Label2:
      now_tokenize_ = ParseType::Label3;
      next_token_ = Token({ L"|",now_pos_,now_pos_ + 1 }, KAGTokenType::SymbolVerticalLine);
      ++now_pos_;
      break;
    case ParseType::Label3:
      ReadLabelHeading();
      break;
    }
    return token;
  }

  KAGTokenType Tokenizer::IsCommandSpecialSymbol(int pos) {
    const auto& str = *str_;
    switch (str[pos]) {
    case L'\n':
      if (now_tokenize_ == ParseType::CommandAtmark)
        return KAGTokenType::SymbolNewLine;
      break;
    case L']':
      if (now_tokenize_ == ParseType::CommandBrancket)
        return KAGTokenType::SymbolCloseCommand;
      break;
    case L'=':  return KAGTokenType::SymbolEqual;
    case L'"':  return KAGTokenType::LiteralString;
    }
    return KAGTokenType::Identifier;
  }

  void Tokenizer::ReadCommand() {
    const auto &str = *str_;
    int s_pos = now_pos_;
    if (now_tokenize_ == ParseType::CommandAtmark) {
      while (IsSpace(str[s_pos]) && str[s_pos] != L'\n') ++s_pos;
    } else {
      while (IsSpace(str[s_pos])) ++s_pos;
    }
    int e_pos = s_pos;

    const auto type = IsCommandSpecialSymbol(s_pos);
    switch (type) {
    case KAGTokenType::SymbolNewLine:
    case KAGTokenType::SymbolCloseCommand:
      now_tokenize_ = ParseType::Normal;
      next_token_ = Token({ str_, s_pos, ++e_pos }, KAGTokenType::SymbolCloseCommand);
      break;

    case KAGTokenType::Identifier:
      ++e_pos;
      while (!IsSpace(str[e_pos]) && IsCommandSpecialSymbol(e_pos) == KAGTokenType::Identifier)++e_pos;
      next_token_ = Token({ str_, s_pos, e_pos }, KAGTokenType::Identifier);
      break;

    case KAGTokenType::SymbolEqual:
      next_token_ = Token({ L"=", s_pos, ++e_pos }, KAGTokenType::SymbolEqual);
      break;

    case KAGTokenType::LiteralString:
    {
      const Char c = str[s_pos];
      do {
        e_pos = static_cast<int>(str.indexOf(c, e_pos + 1));
        if (e_pos == -1) {
          e_pos = static_cast<int>(str.length);
          break;
        }
      } while (str[e_pos - 1] == L'`'); // 文字列がエスケープされている場合は再度検索する

      next_token_ = Token({ str_, s_pos + 1, e_pos }, KAGTokenType::Identifier);
      ++e_pos;
      break;
    }
    }

    now_pos_ = e_pos;
  }

  void Tokenizer::ReadLabel() {
    const auto str = *str_;
    int s_pos = now_pos_;
    int find_pos = static_cast<int>(str.indexOfAny(L"\n|", s_pos));
    if (str[find_pos] == L'|') {
      now_tokenize_ = ParseType::Label2;
    } else {
      now_tokenize_ = ParseType::Normal;
    }
    next_token_ = Token({ str_, s_pos, find_pos }, KAGTokenType::Identifier);
    now_pos_ = find_pos;
  }

  void Tokenizer::ReadLabelHeading() {
    const auto str = *str_;
    int s_pos = now_pos_;
    int find_pos = static_cast<int>(str.indexOf(L"\n", s_pos));
    next_token_ = Token({ str_, s_pos, find_pos }, KAGTokenType::Identifier);
    now_pos_ = find_pos;
    now_tokenize_ = ParseType::Normal;
  }

  void Tokenizer::ReadNormal(KAGTokenType type) {
    const auto &str = *str_;
    int size = static_cast<int>(str_->length);

    //[s_pos, e_pos)
    int s_pos = now_pos_; // start position
    int e_pos = s_pos;    // end position

    if (type == KAGTokenType::SymbolNewLine) {
      do {
        e_pos++;
        type = IsNewLineSpecialSymbol(str, e_pos);
      } while (type == KAGTokenType::SymbolTab);
    }

    switch (type) {
    case KAGTokenType::SymbolOpenCommand:
      now_tokenize_ = ParseType::CommandBrancket;
      ++e_pos;

      break;

    case KAGTokenType::SymbolAtMark:
      type = KAGTokenType::SymbolOpenCommand;
      now_tokenize_ = ParseType::CommandAtmark;
      ++e_pos;
      break;

    case KAGTokenType::SymbolAsterisk:
      now_tokenize_ = ParseType::Label;
      ++e_pos;
      break;

    case KAGTokenType::EscapeText:
      e_pos += 2;
    case KAGTokenType::Text:
      type = KAGTokenType::Text;
      while (e_pos < size) {
        auto type = IsNormalSpecialSymbol(str, e_pos);
        if (type != KAGTokenType::Text && type != KAGTokenType::EscapeText)break;
        ++e_pos;
      }
      break;

    case KAGTokenType::Comment:

      e_pos = static_cast<int>(str.indexOf(L"\n", e_pos + 1));
      break;

    }
    next_token_ = Token({ str_, s_pos, e_pos }, type);
    now_pos_ = e_pos;
  }

}
