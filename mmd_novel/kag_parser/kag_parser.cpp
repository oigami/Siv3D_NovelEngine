#include <kag_parser/kag_parser.h>

namespace kag {

  Parser::Parser(const FilePath & path) {
    TextReader reader(path);
    tokenizer_.Initialize(reader.readAll());
  }

  Parser::Type Parser::nextType() {
    if (tokenizer_.NextToken().Type == KAGTokenType::Text) {
      return Type::Text;
    } else {
      return Type::Command;
    }
  }

  Parser::CommandToken Parser::readCommand() {
    const auto type = tokenizer_.Read().Type;
    SnapShotSpan name;
    CommandToken::Arguments args;
    if (type == KAGTokenType::SymbolOpenCommand) {
      kag::Tokenizer::Token token = tokenizer_.Read();
      if (token != KAGTokenType::Identifier)
        throw std::runtime_error(Narrow(token.Span().Str()));
      name = token.Span();
      token = tokenizer_.Read();
      while (token != KAGTokenType::SymbolCloseCommand) {
        if (token != KAGTokenType::Identifier)throw std::runtime_error
      }
    } else {
      throw std::runtime_error("readCommand error");
    }
    return{ name,std::move(args) };
  }

  Parser::TextToken Parser::readText() {
    return TextToken();
  }

}
