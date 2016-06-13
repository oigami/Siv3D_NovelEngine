#include "kag_parser.h"

namespace kag {

  Parser::Parser(const FilePath & path)
  {
    TextReader reader(path);
    tokenizer_ = Tokenizer(reader.readAll());
  }

  Parser::Parser() :now_line_(0)
  {
  }

  Parser::Type Parser::nextType()
  {
    for ( ;;)
    {
      const KAGTokenType type = tokenizer_.NextToken().Type();
      switch ( type )
      {
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

  Parser::CommandToken Parser::readCommand()
  {
    kag::Tokenizer::Token token = tokenizer_.Read();
    SnapShotSpan name;
    Arguments args;
    if ( token != KAGTokenType::SymbolOpenCommand )
      ShowErrorMsg(token);

    token = tokenizer_.Read();
    if ( token != KAGTokenType::Identifier )
      ShowErrorMsg(token);

    name = token.Span();
    token = tokenizer_.Read();
    while ( token != KAGTokenType::SymbolCloseCommand )
    {
      if ( token != KAGTokenType::Identifier )
        ShowErrorMsg(token);
      std::pair<SnapShotSpan, SnapShotSpan> val;
      val.first = token.Span();

      //=以降がない場合はデフォルトでtrueが指定される
      if ( tokenizer_.NextToken() == KAGTokenType::SymbolEqual )
      {
        tokenizer_.Read();
        token = tokenizer_.NextToken();
        if ( token == KAGTokenType::SymbolCloseCommand )
          val.second = SnapShotSpan(L"", 0, 0, val.first.Line());
        else if ( token != KAGTokenType::Identifier )
          ShowErrorMsg(token);
        else
          val.second = tokenizer_.Read().Span();
      }
      else
      {
        val.second = SnapShotSpan(L"true", 0, 4, val.first.Line());
      }
      token = tokenizer_.Read();
      now_line_ = std::max(now_line_, token.Span().Line());
      args.insert(std::move(val));
    }

    return{ name, std::move(args) };
  }

  Parser::TextToken Parser::readText()
  {
    if ( tokenizer_.NextToken() != KAGTokenType::Text )
      throw std::runtime_error(tokenizer_.NextToken().Span().ToNarrow());
    auto token = tokenizer_.Read().Span();
    now_line_ = std::max(now_line_, token.Line());

    return token;
  }

  void Parser::ShowErrorMsg(const Tokenizer::Token & token) const
  {
    String text = Format(L"構文エラー\n",
                         token.Span().Start(), L"文字目\n syntax error:\n",
                         StringView(token.Span().Str(), token.Span().Length()));
    MessageBox::Show(text);
  }

  Parser::CommandToken::CommandToken(const SnapShotSpan & n, Arguments && args)
    :name_(n), arguments_(std::move(args))
  {
  }

  void Parser::CommandToken::AddIllegalException(const SnapShotSpan & arg_name, const SnapShotSpan & arg_val)
  {
    errors_.push_back({ arg_name,arg_val });
  }

  void Parser::CommandToken::AddNotFoundException(const SnapShotSpan & arg_name)
  {
    errors_.push_back({ arg_name });
  }

  void Parser::CommandToken::AddException(const SnapShotSpan & arg_name, const SnapShotSpan & arg_val)
  {
    AddIllegalException(arg_name, arg_val);
  }

  void Parser::CommandToken::AddException(const SnapShotSpan & arg_name)
  {
    AddNotFoundException(arg_name);
  }

  Parser::Error::Error(const SnapShotSpan & name) :arg_name(name), type(Type::NotFoundArgument) {}

  Parser::Error::Error(const SnapShotSpan & name, const SnapShotSpan & val)
    : arg_name(name), arg_val(val), type(Type::IllegalArgument)
  {
  }
  namespace converter {

    bool TryBool(const SnapShotSpan & span, bool & out)
    {
      if ( span == L"true" )
      {
        out = true;
        return true;
      }
      if ( span == L"false" )
      {
        out = false;
        return true;
      }
      return false;
    }

    bool ToBool(const SnapShotSpan & span)
    {
      bool out = false;
      if ( !TryBool(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryIntRadix(const SnapShotSpan & span, int radix, int & out)
    {
      auto c = span.Str();
      wchar_t *e;
      int ret = wcstol(c, &e, radix);
      if ( e != c + span.Length() )
        return false;
      out = ret;
      return true;
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
    int ToIntRadix(const SnapShotSpan & span, int radix)
    {
      int out = 0;
      if ( !TryIntRadix(span, radix, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryInt10(const SnapShotSpan & span, int& out)
    {
      return TryIntRadix(span, 10, out);
    }

    int ToInt10(const SnapShotSpan & span)
    {
      int out = 0;
      if ( !TryInt10(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryInt16(const SnapShotSpan & span, int & out)
    {
      return TryIntRadix(span, 16, out);
    }

    int ToInt16(const SnapShotSpan & span)
    {
      int out = 0;
      if ( !TryInt16(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryColor(const SnapShotSpan & span, Color & out)
    {
      /* ex) 0x00ff00 */
      if ( span.Length() != 8 ) return false;
      int rgb = ToInt16(span);
      out.b = rgb & 0xff; rgb >>= 8;
      out.g = rgb & 0xff; rgb >>= 8;
      out.r = rgb & 0xff;
      out.a = 255;
      return true;
    }

    Color ToColor(const SnapShotSpan & span)
    {
      Color out;
      if ( !TryColor(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryPage(const SnapShotSpan & span, LayerPage & out)
    {
      if ( span == L"fore" )
      {
        out = LayerPage::Fore;
      }
      else if ( span == L"back" )
      {
        out = LayerPage::Fore;
      }
      else
      {
        return false;
      }
      return true;
    }

    LayerPage ToPage(const SnapShotSpan & span)
    {
      LayerPage out = LayerPage::Fore;
      if ( !TryPage(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    bool TryLayerNum(const SnapShotSpan & span, std::pair<LayerType, int>& out)
    {
      const int len = span.Length();
      if ( len <= 4 )
      {
        if ( span == L"base" )
        {
          out = { LayerType::Background,0 };
          return true;
        }
        else if ( span == L"mmd" )
        {
          out = { LayerType::MMD,0 };
          return true;
        }
        int num = 0;
        bool ok = TryInt10(span, num);
        out = { LayerType::Foreground,num };
        return ok;
      }
      if ( 7 <= len )
      {
        if ( span.substr(0, 7) == L"message" )
        {
          if ( len == 7 )
          {
            out = { LayerType::Message, Define::default };
            return true;
          }
          int num = 0;
          bool ok = TryInt10(span.substr(7, len - 7), num);
          out = { LayerType::Message,num };
          return ok;
        }
      }
      return false;
    }

    std::pair<LayerType, int> ToLayerNum(const SnapShotSpan & span)
    {
      std::pair<LayerType, int> out;
      if ( !TryLayerNum(span, out) )
        throw std::invalid_argument(span.ToNarrow());
      return out;
    }

    template<> int Convert(const SnapShotSpan& val)
    {
      return ToInt10(val);
    }
    template<> std::pair<LayerType, int> Convert(const SnapShotSpan& val)
    {
      return ToLayerNum(val);
    }
    template<> LayerPage Convert(const SnapShotSpan& val)
    {
      return ToPage(val);
    }
    template<> bool Convert(const SnapShotSpan& val)
    {
      return ToBool(val);
    }

    template<> Color Convert(const SnapShotSpan& val)
    {
      return ToColor(val);
    }
    template<> SnapShotSpan Convert(const SnapShotSpan& val)
    {
      return val;
    }


    template<> bool TryConvert(const SnapShotSpan& val, int& out)
    {
      return TryInt10(val, out);
    }
    template<> bool TryConvert(const SnapShotSpan& val, std::pair<LayerType, int>& out)
    {
      return TryLayerNum(val, out);
    }
    template<> bool TryConvert(const SnapShotSpan& val, LayerPage& out)
    {
      return TryPage(val, out);
    }
    template<> bool TryConvert(const SnapShotSpan& val, bool& out)
    {
      return TryBool(val, out);
    }

    template<> bool TryConvert(const SnapShotSpan& val, Color& out)
    {
      return TryColor(val, out);
    }
    template<> bool TryConvert(const SnapShotSpan& val, SnapShotSpan& out)
    {
      out = val;
      return true;
    }


  }

  std::pair<Parser::Arguments::arguments_type::iterator, bool>
    Parser::Arguments::insert(std::pair<SnapShotSpan, SnapShotSpan>&& p)
  {
    return args.insert(std::move(p));
  }

}
