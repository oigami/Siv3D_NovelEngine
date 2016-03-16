#pragma once
#include <Siv3D.hpp>
#include <kag_tokenizer/tokenizer.h>
namespace kag {
  class Parser {
  public:
    enum class Type {
      Command,
      Text,
    };
    using TextToken = SnapShotSpan;
    class CommandToken {
    public:

      using Arguments = std::vector<std::pair<SnapShotSpan, SnapShotSpan>>;
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

    Type nextType();

    CommandToken readCommand();
    TextToken readText();
  private:

    kag::Tokenizer tokenizer_;

  };
}
