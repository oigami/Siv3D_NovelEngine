#pragma once
#include <Siv3D.hpp>
#include <kag_tokenizer/tokenizer.h>
namespace kag {
  enum class FindAttributeResult {
    NotName,
    TypeIsIncorrect,
    Ok,
  };
  class Parser {
  public:
    enum class Type {
      EndOfStream,
      Command,
      Text,
    };

    using TextToken = SnapShotSpan;

    class  Arguments {
      using arguments_type = std::map<SnapShotSpan, SnapShotSpan>;
    public:
      std::pair<arguments_type::iterator, bool> insert(std::pair<SnapShotSpan, SnapShotSpan>&& p) {
        return args.insert(std::move(p));
      }
      template<class T>
      struct Result {
        T val;

        /// <summary>
        /// 検索した属性があった場合true
        /// </summary>
        FindAttributeResult result;
      };


      template<class Func, class Func2>
      void AttributeVal(const SnapShotSpan& name, Func find_f, Func2 not_found_f) const {
        auto it = args.find(name);
        if (it == args.end())  not_found_f(FindAttributeResult::NotName);
        else find_f(it->second);
      }

      /// <summary>
      /// 属性の値を関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class Func> void AttributeVal(const SnapShotSpan& name, Func find_f) const {
        auto it = args.find(name);
        if (it == args.end()) return;
        return find_f(it->second);
      }

      template<class T> Result<Optional<T>> AttributeValTo(const SnapShotSpan& name) const {
        auto it = args.find(name);
        if (it == args.end()) return{ none, FindAttributeResult::NotName };
        auto opt = ToCast<T>(it->second);
        return{ opt , !opt ? FindAttributeResult::TypeIsIncorrect : FindAttributeResult::Ok };
      }

      template<class T, class Func, class Func2>
      void AttributeValTo(const SnapShotSpan& name, Func find_f, Func2 not_found_f) const {
        AttributeVal(name, [&](const SnapShotSpan& val) {
          Optional<T> opt = ToCast<T>(val);
          if (opt) {
            find_f(*opt);
          } else {
            not_found_f(FindAttributeResult::TypeIsIncorrect);
          }
        }, not_found_f);
      }

      /// <summary>
      /// 属性の値を型変換して関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// <para>型変換に失敗した場合は例外を投げる</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class T, class Func>
      void AttributeValTo(const SnapShotSpan& name, Func find_f) const {
        AttributeVal(name, [&](const SnapShotSpan& val) {
          Optional<T> opt = ToCast<T>(val);
          if (opt) {
            find_f(*opt);
          }
        });
      }
      /*  Result<SnapShotSpan> AttributeVal(const SnapShotSpan& name) const {
          auto it = args.find(name);
          if (it == args.end()) return{ L"",  FindAttributeResult::NotName };
          return{ it->second, FindAttributeResult::Ok };
        }*/

    private:
      arguments_type args;
    };
    class CommandToken {
    public:

      CommandToken(const SnapShotSpan& n, Arguments&& args)
        :name_(n), arguments_(std::move(args)) {
      }

      const SnapShotSpan& name() const { return name_; }
      const Arguments& arguments() const { return arguments_; }

    private:

      SnapShotSpan name_;
      Arguments arguments_;
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
