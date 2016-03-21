#pragma once
#include <Siv3D.hpp>
#include <kag_tokenizer/tokenizer.h>
namespace kag {
  enum class FindAttributeResult {
    NotName,
    TypeIsIncorrect,
    Ok,
  };
  struct Default {
    constexpr operator int() const { return -1; }
  };
  constexpr Default default;

  template<class Type> class Value {
  public:
    constexpr Value(const Type& v) :t(v) {}
    constexpr Value() : t(kag::default) {}
    constexpr operator const Type&() const { return t; }
    Type& operator=(const Type& v) {
      t = v;
      return t;
    }
    constexpr bool operator==(const Default& def) const {
      return t == def;
    }
  private:

    Type t;
  };
  template<class Type> constexpr bool operator==(const Type& t, const Default&) {
    return t == static_cast<Type>(kag::default);
  }
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
    inline int ToInt(const SnapShotSpan& span, int radix) {
      auto c = span.Str();
      wchar_t *e;
      int ret = wcstol(c, &e, radix);
      if (e != c + span.Length())
        throw std::runtime_error(span.ToNarrow());
      return ret;
    }

    inline int ToInt10(const SnapShotSpan& span) {
      return ToInt(span, 10);
    }

    /// <summary>
    /// 値がL"default"の時に指定された値を返す関数を返す
    /// <para>L"default"以外の時は通常のToInt10を呼び出す</para>
    /// </summary>
    /// <param name="default_val"></param>
    /// <returns></returns>
    inline auto ToInt10(int default_val) {
      return [=](const SnapShotSpan& span) {
        if (span == L"default") return default_val;
        return ToInt10(span);
      };
    }

    inline int ToInt16(const SnapShotSpan& span) {
      return ToInt(span, 16);
    }

    inline Color ToColor(const SnapShotSpan& span) {
      //ex) 0x00ff00
      if (span.Length() != 8) throw std::runtime_error(span.ToNarrow());
      Color ret;
      int rgb = ToInt16(span);
      ret.b = rgb & 0xff; rgb >>= 8;
      ret.g = rgb & 0xff; rgb >>= 8;
      ret.r = rgb & 0xff;
      return ret;
    }

    inline int ToPageNum(const SnapShotSpan& span) {
      if (span == L"fore") return 0;
      if (span == L"back") return 1;
      throw std::runtime_error(span.ToNarrow());
    }

    enum class LayerType {
      Message,
      Foreground,
      Background,
    };

    inline std::pair<LayerType, int> ToLayerNum(const SnapShotSpan& span) {
      const int len = span.Length();
      if (len <= 4) {
        if (span == L"base") {
          return{ LayerType::Background,0 };
        }
        return{ LayerType::Foreground, ToInt10(span) };
      }
      if (7 <= len) {
        if (span.substr(0, 7) == L"message") {
          if (len == 7) {
            return{ LayerType::Message, default };
          }
          return{ LayerType::Message, ToInt10(span.substr(7,len - 7)) };
        }
      }
      throw std::runtime_error(span.ToNarrow());
    }

    inline int ToMessageLayerNum(const SnapShotSpan& span) {
      auto ret = ToLayerNum(span);
      if (ret.first != LayerType::Message)throw std::runtime_error(span.ToNarrow());
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

      /// <summary>
      /// 属性の値を関数に渡す
      /// <para>見つからなかった場合はふたつ目の関数を呼び出す</para>
      /// </summary>
      /// <param name="name">属性の名前</param>
      /// <param name="find_f">見つかった場合に呼ばれる関数</param>
      /// <param name="not_found_f">見つからなかった時に呼ばれる関数</param>
      template<class Func, class Func2>
      void AttributeVal(const SnapShotSpan& name, Func find_f, Func2 not_found_f) const {
        auto it = args.find(name);
        if (it == args.end()) not_found_f();
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

      /// <summary>
      /// 属性の値を指定した関数で型変換して関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class Converter, class Func>
      void AttributeValTo(const SnapShotSpan& name, Converter(*c)(const SnapShotSpan&), Func find_f) const {
        AttributeVal(name, [&](const SnapShotSpan& val) {
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
      void AttributeValTo(const SnapShotSpan& name, Converter c, Func find_f) const {
        AttributeVal(name, [&](const SnapShotSpan& val) {
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
      const Arguments& arguments() const { return arguments_; }

    private:

      SnapShotSpan name_;
      Arguments arguments_;
    };

    explicit Parser(const FilePath& path);

    Parser() = default;

    Type nextType();

    CommandToken readCommand();
    TextToken readText();

    void ShowErrorMsg(const Tokenizer::Token& token) const;


  private:

    kag::Tokenizer tokenizer_;

  };


}
