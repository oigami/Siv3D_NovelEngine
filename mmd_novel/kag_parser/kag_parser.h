#pragma once
#include <Siv3D.hpp>
#include "tokenizer.h"
#include <MmdNovel/default_value.h>
#include <MmdNovel/layer.h>
namespace kag {
  enum class FindAttributeResult
  {
    NotName,
    TypeIsIncorrect,
    Ok,
  };

  namespace converter {

    bool TryBool(const SnapShotSpan& span, bool& out);
    bool ToBool(const SnapShotSpan& span);

    bool TryIntRadix(const SnapShotSpan& span, int radix, int& out);

    /// <summary>
    ///
    /// </summary>
    /// <param name="span"></param>
    /// <param name="radix">
    /// <para>0 : 先頭を見て変換</para>
    /// <para>2以上 : n進数として変換</para>
    /// </param>
    /// <returns></returns>
    int ToIntRadix(const SnapShotSpan& span, int radix);

    bool TryInt10(const SnapShotSpan& span, int& out);
    int ToInt10(const SnapShotSpan& span);

    bool TryInt16(const SnapShotSpan& span, int& out);
    int ToInt16(const SnapShotSpan& span);

    bool TryColor(const SnapShotSpan& span, Color& out);
    Color ToColor(const SnapShotSpan& span);

    bool TryPage(const SnapShotSpan& span, LayerPage& out);
    LayerPage ToPage(const SnapShotSpan& span);
    enum class LayerType
    {
      Message,
      Foreground,
      Background,
      MMD,
    };

    bool TryLayerNum(const SnapShotSpan& span, std::pair<LayerType, int>& out);
    std::pair<LayerType, int> ToLayerNum(const SnapShotSpan& span);

    bool TryMessageLayerNum(const SnapShotSpan& span, int& out);
    int ToMessageLayerNum(const SnapShotSpan& span);

    template<class T> T Convert(const SnapShotSpan& val);
    template<class T> bool TryConvert(const SnapShotSpan& val, T& out);

  }

  class Parser
  {
  public:
    enum class Type
    {
      EndOfStream,
      Command,
      Text,
    };

    using TextToken = SnapShotSpan;

    template<class T>struct Must
    {
      T& operator*() { return val; }
      T* operator->() { return &val; }

      const T& operator*() const { return val; }
      const T* operator->() const { return &val; }
    private:

      T val;
    };


    class  Arguments
    {
      using arguments_type = std::map<SnapShotSpan, SnapShotSpan>;
      Arguments(const Arguments&) = delete;
      void operator=(const Arguments&) = delete;
    public:
      Arguments() = default;
      Arguments(Arguments&&) = default;
      std::pair<arguments_type::iterator, bool> insert(std::pair<SnapShotSpan, SnapShotSpan>&& p);

      void IfNotEmptyException() const;

      SnapShotSpan find_or_throw(const SnapShotSpan& name);

      /// <summary>
      /// 属性の値を関数に渡す
      /// <para>見つからなかった場合はふたつ目の関数を呼び出す</para>
      /// </summary>
      /// <param name="name">属性の名前</param>
      /// <param name="find_f">見つかった場合に呼ばれる関数</param>
      /// <param name="not_found_f">見つからなかった時に呼ばれる関数</param>
      template<class Func, class Func2 = void(*)()>
      void Val(const SnapShotSpan& name, Func find_f, Func2 not_found_f = []() {})
      {
        auto it = args.find(name);
        if ( it == args.end() )
        {
          not_found_f();
          return;
        }
        auto val = std::move(it->second);
        args.erase(it);
        find_f(val);
      }

      template<class Converter, class T> T ValOrDefaultTo(const SnapShotSpan& name, Converter c, T default_val)
      {
        auto it = args.find(name);
        if ( it == args.end() )
          return default_val;
        auto val = std::move(it->second);
        args.erase(it);
        return c(val);
      }

      SnapShotSpan ValOrDefault(const SnapShotSpan& name, const SnapShotSpan& default_val);

      template<class T> Arguments& get(const SnapShotSpan& name, T& val)
      {
        Val(name, [&val](const SnapShotSpan& v) { val = converter::Convert<T>(v); });
        return *this;
      }

      template<class T> Arguments& get(const SnapShotSpan& name, Optional<T>& val)
      {
        Val(name, [&val](const SnapShotSpan& v) { val = converter::Convert<T>(v); });
        return *this;
      }

      template<class T> Arguments& get(const SnapShotSpan& name, Must<T>& val)
      {
        Val(name,
            [&val](const SnapShotSpan& v) { *val = converter::Convert<T>(v); },
            []() { throw std::runtime_error(""); });
        return *this;
      }

      /// <summary>
      /// 属性の値を指定した関数で型変換して関数に渡す
      /// <para>見つからなかった場合は何もしない</para>
      /// </summary>
      /// <param name="name"></param>
      /// <param name="find_f"></param>
      template<class Converter, class Func> void ValTo(const SnapShotSpan& name, Converter c, Func find_f)
      {
        Val(name, [&find_f, &c, &val](const SnapShotSpan& val) { find_f(c(val)); });
      }

    private:
      arguments_type args;
    };

    struct Error
    {
      enum class Type
      {
        NotFoundArgument,
        IllegalArgument,
        NotFoundTag
      };

      Error(const SnapShotSpan& name);
      Error(const SnapShotSpan& name, const SnapShotSpan& val);

      Type type = Type::NotFoundTag;
      SnapShotSpan arg_name;
      SnapShotSpan arg_val;
    };

    class CommandToken
    {
    public:

      CommandToken(const SnapShotSpan& n, Arguments&& args);

      const SnapShotSpan& name() const { return name_; }
      Arguments& arguments() { return arguments_; }

      void AddIllegalException(const SnapShotSpan& arg_name, const SnapShotSpan& arg_val);
      void AddNotFoundException(const SnapShotSpan& arg_name);

    private:

      Array<Error> errors_;

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
