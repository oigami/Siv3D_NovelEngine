#include <kag_parser/kag_parser.h>

namespace kag {

  Parser::Parser(const FilePath & path) {
    TextReader reader(path);
    tokenizer_ = Tokenizer(reader.readAll());
  }

  Parser::Type Parser::nextType() {
    for (;;) {
      const KAGTokenType type = tokenizer_.NextToken().Type();
      switch (type) {
      case KAGTokenType::EndOfStream:
        return Type::EndOfStream;
      case KAGTokenType::Text:
        return Type::Text;
      case KAGTokenType::SymbolOpenCommand:
        return Type::Command;
      default:
        tokenizer_.Read();
        break;
      }
    }
  }

  Parser::CommandToken Parser::readCommand() {
    kag::Tokenizer::Token token = tokenizer_.Read();
    SnapShotSpan name;
    Arguments args;
    if (token != KAGTokenType::SymbolOpenCommand)
      ShowErrorMsg(token);

    token = tokenizer_.Read();
    if (token != KAGTokenType::Identifier)
      ShowErrorMsg(token);

    name = token.Span();
    token = tokenizer_.Read();
    while (token != KAGTokenType::SymbolCloseCommand) {
      if (token != KAGTokenType::Identifier)
        ShowErrorMsg(token);
      std::pair<SnapShotSpan, SnapShotSpan> val;
      val.first = token.Span();

      //=以降がない場合はデフォルトでtrueが指定される
      if (tokenizer_.NextToken() == KAGTokenType::SymbolEqual) {
        tokenizer_.Read();
        token = tokenizer_.Read();
        if (token != KAGTokenType::Identifier)
          ShowErrorMsg(token);
        val.second = token.Span();
      } else {
        val.second = SnapShotSpan(L"true");
      }
      token = tokenizer_.Read();
      args.insert(std::move(val));
    }

    return{ name, std::move(args) };
  }

  Parser::TextToken Parser::readText() {
    if (tokenizer_.NextToken() != KAGTokenType::Text)
      throw std::runtime_error(tokenizer_.NextToken().Span().ToNarrow());

    return tokenizer_.Read().Span();
  }

  void Parser::ShowErrorMsg(const Tokenizer::Token & token) const {
    String text = Format(L"構文エラー\n",
      token.Span().Start(), L"文字目\n syntax error:\n",
      StringView(token.Span().Str(), token.Span().Length()));
    MessageBox::Show(text);
  }

}
