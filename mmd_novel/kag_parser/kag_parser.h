#pragma once
#include <Siv3D.hpp>
#include <kag_tokenizer/tokenizer.h>
namespace kag {
  class Parser {
  public:
    enum class Type {
      EndOfStream,
      Command,
      Text,
    };

    using TextToken = SnapShotSpan;
    class CommandToken {
    public:

      using Arguments = std::map<SnapShotSpan, SnapShotSpan>;
      CommandToken(const SnapShotSpan& n, Arguments&& args)
        :name_(n), arguments_(std::move(args)) {
      }

      const SnapShotSpan& name() const { return name_; }
      const Arguments& arguments() const { return arguments_; }

    private:

      SnapShotSpan name_;
      Arguments  arguments_;
    };

    explicit Parser(const FilePath& path);

    Parser();

    Type nextType();

    CommandToken readCommand();
    TextToken readText();

    void ShowErrorMsg(const Tokenizer::Token& token) const;


  private:

    kag::Tokenizer tokenizer_;

  };
}
