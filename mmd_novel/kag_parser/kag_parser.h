#pragma once
#include <Siv3D.hpp>
#include "tokenizer.h"
#include <MmdNovel/default_value.h>
#include <MmdNovel/layer.h>
namespace kag {
  enum class FindAttributeResult {
    NotName,
    TypeIsIncorrect,
    Ok,
  };

  /// <summary>
  /// 変換失敗時に例外を投げる
  /// </summary>
  namespace converter {
    inline bool ToBool(const SnapShotSpan& span) {
      if (span == L"true") return true;
      if (span == L"false") return false;
      throw std::runtime_error(span.ToNarrow());
    }

    /// <summary>
    ///
    /// </summary>
    /// <param name="span"></param>
    /// <param name="radix">
    /// <para>0 : 先頭を見て変換</para>
    /// <para>2以上 : n進数として変換</para>
    /// </param>
    /// <returns></returns>
    inline int ToIntRadix(const SnapShotSpan& span, int radix) {
      auto c = span.Str();
      wchar_t *e;
      int ret = wcstol(c, &e, radix);
      if (e != c + span.Length())
        throw std::runtime_error(span.ToNarrow());
      return ret;
    }

    inline int ToInt10(const SnapShotSpan& span) {
      return ToIntRadix(span, 10);
    }

    inline int ToInt16(const SnapShotSpan& span) {
      return ToIntRadix(span, 16);
    }

    inline Color ToColor(const SnapShotSpan& span) {
      /* ex) 0x00ff00 */
      if (span.Length() != 8) throw std::runtime_error(span.ToNarrow());
      Color ret;
      int rgb = ToInt16(span);
      ret.b = rgb & 0xff; rgb >>= 8;
      ret.g = rgb & 0xff; rgb >>= 8;
      ret.r = rgb & 0xff;
      ret.a = 255;
      return ret;
    }

    inline int ToPageNum(const SnapShotSpan& span) {
      if (span == L"fore") return Define::fore_page;
      if (span == L"back") return Define::back_page;
      throw std::runtime_error(span.ToNarrow());
    }

    inline LayerPage ToPage(const SnapShotSpan& span) {
      return ToPageNum(span) == Define::fore_page ? LayerPage::Fore : LayerPage::Back;
    }
    enum class LayerType {
      Message,
      Foreground,
      Background,
      MMD,
    };

    inline std::pair<LayerType, int> ToLayerNum(const SnapShotSpan& span) {
      const int len = span.Length();
      if (len <= 4) {
        if (span == L"base") {
          return{ LayerType::Background,0 };
        } else if (span == L"mmd") {
          return{ LayerType::MMD,0 };
        }
        return{ LayerType::Foreground, ToInt10(span) };
      }
      if (7 <= len) {
        if (span.substr(0, 7) == L"message") {
          if (len == 7) {
            return{ LayerType::Message, Define::default };
          }
          return{ LayerType::Message, ToInt10(span.substr(7,len - 7)) };
        }
      }
      throw std::runtime_error(span.ToNarrow());
    }

    inline int ToMessageLayerNum(const SnapShotSpan& span) {
      auto ret = ToLayerNum(span);
      if (ret.first != LayerType::Message) throw std::runtime_error(span.ToNarrow());
      return ret.second;
    }
  }

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

      void IfNotEmptyException() const {
        if (args.size()) throw std::runtime_error(args.begin()->first.ToNarrow());
      }

      SnapShotSpan find_or_throw(const SnapShotSpan& name) {
        auto it = args.find(name);
        if (it == args.end()) throw std::runtime_error(name.ToNarrow());
        auto val = std::move(it->second);
        args.erase(it);
        return val;
      }

      /// <summary>
      /// 属性の値を関数に渡す
      /// <para>見つからなかった場合はふたつ目の関数を呼び出す</para>
      /// </summary>
      /// <param name="name">属性の名前</param>
      /// <param name="find_f">見つかった場合に呼ばれる関数</param>
      /// <param name="not_found_f">見つからなかった時に呼ばれる関数</param>
      template<class Func, class Func2 = void(*)()>
      void Val(const SnapShotSpan& name, Func find_f, Func2 not_found_f = []() {}) {
        auto it = args.find(name);
        if (it == args.end()) not_found_f();
        else {
          auto val = std::move(it->second);
          args.erase(it);
          find_f(val);
        }
      }


      template<class Converter, class T>
      T ValOrDefaultTo(const SnapShotSpan& name, Converter c, T default_val) {
        auto it = args.find(name);
        if (it == args.end())
          return default_val;
        auto val = std::move(it->second);
        args.erase(it);
        return c(val);
      }
      template<class T>
      T ValOrDefault(const SnapShotSpan& name, T default_val) {
        return ValOrDefaultTo(name, [](auto& v) {return v; }, default_val);
      }

      /// <summary>
      /// 属性の値を指定した関数で型変換して関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class Converter, class Func>
      void ValTo(const SnapShotSpan& name, Converter(*c)(const SnapShotSpan&), Func find_f) {
        Val(name, [&](const SnapShotSpan& val) {
          find_f(c(val));
        });
      }

      /// <summary>
      /// 属性の値を指定した関数で型変換して関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class Converter, class Func>
      void ValTo(const SnapShotSpan& name, Converter c, Func find_f) {
        Val(name, [&](const SnapShotSpan& val) {
          find_f(c(val));
        });
      }

    private:
      arguments_type args;
    };
    class CommandToken {
    public:

      CommandToken(const SnapShotSpan& n, Arguments&& args)
        :name_(n), arguments_(std::move(args)) {
      }

      const SnapShotSpan& name() const { return name_; }
      Arguments& arguments() { return arguments_; }

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

    int NowLine()const { return now_line_; }

  private:
    int now_line_;
    kag::Tokenizer tokenizer_;

  };


}
