#pragma once
#include <Siv3D.hpp>
#include "tokenizer.h"
#include <MmdNovel/default_value.h>
#include <MmdNovel/kag_token.h>
namespace kag
{
  enum class FindAttributeResult
  {
    NotName,
    TypeIsIncorrect,
    Ok,
  };


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

    explicit Parser(const FilePath& path);

    Parser();

    /// <summary>
    /// 次に返すトークンの種類を返す
    /// </summary>
    /// <returns></returns>
    Type nextType();

    /// <summary>
    /// 次のコマンドを返す
    /// <para>nextType()で判定したあとに使う</para>
    /// </summary>
    /// <returns></returns>
    CommandToken readCommand();

    /// <summary>
    /// 次のテキストを返す
    /// <para>nextType()で判定したあとに使う</para>
    /// </summary>
    /// <returns></returns>
    TextToken readText();

    void ShowErrorMsg(const Tokenizer::Token& token) const;

    /// <summary>
    /// 処理をしている行を返す
    /// </summary>
    /// <returns></returns>
    int NowLine()const { return now_line_; }

  private:
    int now_line_;
    kag::Tokenizer tokenizer_;

  };


}
