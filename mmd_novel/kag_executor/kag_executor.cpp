#include "kag_executor.h"
namespace kag {
  Executor::Executor(const FilePath & path) :parser_(path) {
    tag_func_[SnapShotSpan(L"l")] = &Executor::LTag;
    tag_func_[SnapShotSpan(L"r")] = &Executor::RTag;
    tag_func_[SnapShotSpan(L"p")] = &Executor::PTag;
    tag_func_[SnapShotSpan(L"delay")] = &Executor::DelayTag;
    tag_func_[SnapShotSpan(L"er")] = &Executor::ERTag;
    tag_func_[SnapShotSpan(L"cm")] = &Executor::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &Executor::CTTag;
    Clear();
  }
  void Executor::Clear() {
    is_wait_click_ = false;
    is_click_new_page = false;
    message_manager_.Clear();
  }

  void Executor::Update() {
    if (!message_manager_.Update()) {
      if (Input::MouseL.clicked)
        message_manager_.NextPage();
      return;
    } else if (message_manager_.IsFlush() == false) {
      if (Input::MouseL.clicked)
        message_manager_.Flush();
      return;
    }
    if (is_wait_click_) {
      if (!Input::MouseL.clicked)
        return;
      is_wait_click_ = false;
      if (is_click_new_page) {
        is_click_new_page = false;
        message_manager_.NextPage();
      }
    }

    while (!is_wait_click_) {
      switch (parser_.nextType()) {
      case  kag::Parser::Type::Text:
        message_manager_.Append(parser_.readText());
        return;

      case  kag::Parser::Type::Command:
      {
        auto token = parser_.readCommand();
        auto func = tag_func_[token.name()];
        if (func) {
          (this->*func)(token);
        }
        break;
      }

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  void Executor::Draw() {
    message_manager_.Draw();
  }



  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
                                      タグの実装
   - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void Executor::LTag(const Parser::CommandToken &) {
    is_wait_click_ = true;
  }

  void Executor::RTag(const Parser::CommandToken &) {
    message_manager_.AppendNewLine();
  }

  void Executor::PTag(const Parser::CommandToken &) {
    is_wait_click_ = true;
  }

  enum class FindAttributeResult {
    NotName,
    TypeIsIncorrect,
    Ok,
  };
  template<class T> std::pair<FindAttributeResult, T> FindAttributeValue(const Parser::CommandToken::Arguments& args,
    const SnapShotSpan& attribute_name);

  template<>std::pair<FindAttributeResult, int>
    FindAttributeValue<int>(
      const Parser::CommandToken::Arguments& args,
      const SnapShotSpan& attribute_name) {

      auto attr = args.find(attribute_name);
      if (attr == args.end())
        return{ FindAttributeResult::NotName, 0 };

      auto i = attr->second.ToInt();
      if (!i.has_value())
        return{ FindAttributeResult::TypeIsIncorrect,0 };
      return{ FindAttributeResult::Ok,*i };
    }

    void Executor::DelayTag(const Parser::CommandToken & token) {
      auto& arguments = token.arguments();
      auto res = FindAttributeValue<int>(arguments, L"speed");
      if (res.first == FindAttributeResult::Ok) {
        message_manager_.SetDelayTime(res.second);
      } else {
        ShowErrorMsg(L"[delay]のエラー");
      }
    }

    void Executor::ERTag(const Parser::CommandToken & token) {
      message_manager_.Clear();
    }

    void Executor::CMTag(const Parser::CommandToken & token) {
      message_manager_.AllClear();
    }

    void Executor::CTTag(const Parser::CommandToken & token) {
      CMTag(token);
      message_manager_.SetCurrent(0, LayerPage::Fore);
    }

}
