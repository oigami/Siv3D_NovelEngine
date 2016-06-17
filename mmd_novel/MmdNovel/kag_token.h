#pragma once
#include <map>
#include <MmdNovel/message/snapshotspan.h>
namespace kag
{
  class LayerPage;
  namespace converter
  {
    using LayerType = SnapShotSpan;

    /// <summary>
    /// 文字列をブール値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryBool(const SnapShotSpan& span, bool& out);

    /// <summary>
    /// 文字列をradix進数と解釈して整数値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryIntRadix(const SnapShotSpan& span, int radix, int& out);

    /// <summary>
    /// 文字列を整数値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryInt10(const SnapShotSpan& span, int& out);

    /// <summary>
    /// 文字列を16進数と解釈して整数値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryInt16(const SnapShotSpan& span, int& out);

    /// <summary>
    /// 文字列をカラー値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryColor(const SnapShotSpan& span, Color& out);

    /// <summary>
    /// 文字列をLayerPageに変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryPage(const SnapShotSpan& span, LayerPage& out);

    /// <summary>
    /// 文字列をLayerNumに変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    bool TryLayerNum(const SnapShotSpan& span, std::pair<LayerType, int>& out);

    /// <summary>
    /// 文字列をブール値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    bool ToBool(const SnapShotSpan& span);

    /// <summary>
    ///
    /// </summary>
    /// <param name="span"></param>
    /// <param name="radix">
    /// <para>0 : 先頭を見て変換</para>
    /// <para>2以上 : n進数として変換</para>
    /// </param>
    /// <returns>変換した値</returns>
    int ToIntRadix(const SnapShotSpan& span, int radix);

    /// <summary>
    /// 文字列を整数値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    int ToInt10(const SnapShotSpan& span);

    /// <summary>
    /// 文字列を整数値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    int ToInt16(const SnapShotSpan& span);

    /// <summary>
    /// 文字列をカラー値に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    Color ToColor(const SnapShotSpan& span);

    /// <summary>
    /// 文字列をLayerPageに変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    LayerPage ToPage(const SnapShotSpan& span);

    /// <summary>
    /// 文字列をLayerTypeに変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    std::pair<LayerType, int> ToLayerNum(const SnapShotSpan& span);

    /// <summary>
    /// 文字列を各種型に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <exception cref="std::invalid_argument">変換に失敗した時</exception>
    /// <returns>変換した値</returns>
    template<class T> T Convert(const SnapShotSpan& val);

    /// <summary>
    /// 文字列を各種型に変換する
    /// </summary>
    /// <param name="span">文字列</param>
    /// <param name="out">代入する変数</param>
    /// <returns>変換の成否</returns>
    template<class T> bool TryConvert(const SnapShotSpan& val, T& out);

  }

  template<class T>struct Must
  {
    Must() = default;
    Must(const T& t) :val(t) {}

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

    /// <summary>
    /// 属性を取得する
    /// </summary>
    /// <param name="name">属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <param name="error_func">型変換失敗時に呼ばれる関数</param>
    /// <returns></returns>
    template<class T, class Func> Arguments& get(const SnapShotSpan& name, T& val, Func error_func)
    {
      Val(name, [&error_func, &name, &val](const SnapShotSpan& v)
      {
        if ( !converter::TryConvert<T>(v, val) )
          error_func(name, v);
      });
      return *this;
    }

    /// <summary>
    /// 属性を取得する
    /// <para>属性がない場合は何もしない</para>
    /// </summary>
    /// <param name="name">属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <param name="error_func">型変換失敗時に呼ばれる関数</param>
    /// <returns></returns>
    template<class T, class Func> Arguments& get(const SnapShotSpan& name, Optional<T>& val, Func error_func)
    {
      Val(name, [&error_func, &name, &val](const SnapShotSpan& v)
      {
        T res;
        if ( !converter::TryConvert<T>(v, res) )
          error_func(name, v);
        else
          val = Optional<T>(res);
      });
      return *this;
    }

    /// <summary>
    /// 属性を取得する
    /// <para>属性がない場合は何もしない</para>
    /// </summary>
    /// <param name="name">属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <param name="error_func">型変換失敗時に呼ばれる関数</param>
    /// <returns></returns>
    template<class T, class Func> Arguments& get(const SnapShotSpan& name, Value<T>& val, Func error_func)
    {
      get(name, *val, error_func);
      return *this;
    }

    /// <summary>
    /// 属性を取得する
    /// <para>属性がない場合は何もしない</para>
    /// </summary>
    /// <param name="name">属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <param name="error_func">属性が存在しなかった時、型変換失敗時に呼ばれる関数</param>
    /// <returns></returns>
    template<class T, class Func> Arguments& get(const SnapShotSpan& name, Must<T>& val, Func error_func)
    {
      Val(name, [&error_func, &name, &val](const SnapShotSpan& v)
      {
        if ( !converter::TryConvert<T>(v, *val) )
          error_func(name, v);
      },
        [&error_func, &name]() { error_func(name); });
      return *this;
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

    /// <summary>
    /// 型変換エラー時に呼ぶ関数
    /// </summary>
    /// <param name="arg_name">失敗した属性の名前</param>
    /// <param name="arg_val">失敗した属性の値</param>
    void AddIllegalException(const SnapShotSpan& arg_name, const SnapShotSpan& arg_val);

    /// <summary>
    ///必須属性がなかった時に呼ぶ関数
    /// </summary>
    /// <param name="arg_name">なかった属性の名前</param>
    void AddNotFoundException(const SnapShotSpan& arg_name);

    void AddException(const SnapShotSpan& arg_name, const SnapShotSpan& arg_val);
    void AddException(const SnapShotSpan& arg_name);

    /// <summary>
    /// 属性値を取得する
    /// </summary>
    /// <param name="name">取得する属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <returns></returns>
    template<class T> CommandToken& get(const SnapShotSpan& name, T& val)
    {
      arguments_.get(name, val, [this](auto... args) { AddException(args...); });
      return *this;
    }

    /// <summary>
    /// 属性値を取得する
    /// </summary>
    /// <param name="name">取得する属性の名前</param>
    /// <param name="val">代入する変数</param>
    /// <returns></returns>
    template<class T> CommandToken& get(const SnapShotSpan& name, Must<T>& val)
    {
      arguments_.get(name, val, [this](auto... args) { AddException(args...); });
      return *this;
    }

    /// <summary>
    /// エラーが発生している時trueを返す
    /// </summary>
    /// <returns></returns>
    bool HasError() const { return !errors_.empty(); }

    /// <summary>
    /// エラー内容を返す
    /// </summary>
    /// <returns></returns>
    const Array<Error>& Errors() const { return errors_; }

  private:

    Array<Error> errors_;

    SnapShotSpan name_;
    Arguments arguments_;
  };

}
