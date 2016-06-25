#pragma once
#include <MmdNovel/kag_executor.h>
#include <kag_parser/kag_parser.h>
#include <MmdNovel/imanager.h>

namespace kag
{
  namespace file
  {
    namespace detail
    {
      struct IFileManager : IManager
      {
        using IManager::IManager;
        using FuncList = std::map<SnapShotSpan, std::function<void(CommandToken&)>>;
        virtual void AddTag(FuncList& func_list) = 0;
      };
    }
    template<class T> struct IFileManagerType :public detail::IFileManager, public std::enable_shared_from_this<T>
    {
      using Ptr = std::shared_ptr<T>;

      using IFileManager::IFileManager;
      template<class Manager> auto Bind(void (Manager::*f)(CommandToken&))
      {
        return[this_ = shared_from_this(), f](CommandToken& token){(this_.get()->*f)(token); };
      }
    };

    class  FileExecutor : public Executor
    {
    public:
      FileExecutor();
      FileExecutor(const FilePath& path);
      void Load(const FilePath& path);
      void Update();
      int NowFileLine() const;

      using TagFunction = detail::IFileManager::FuncList;

      void AddManager(const SnapShotSpan & name, const std::shared_ptr<detail::IFileManager>& manager);

      template<class Manager> void AddManager(const SnapShotSpan& name)
      {
        static_assert(std::is_base_of<detail::IFileManager, Manager>::value, "ManagerはIFileManagerを継承している必要があります");
        static_assert(std::is_convertible<Manager*, detail::IFileManager*>::value, "Managerがpublic継承しているか確認してください");
        AddManager(name, std::make_shared<Manager>(this->shared_from_this()));
      }

      int NowLine() const;

    private:
      TagFunction tag_func_;

      Parser parser_;

      //タグリファレンス
      //http://devdoc.kikyou.info/tvp/docs/kag3doc/contents/index.html

      //メッセージ関連

      //void UnlockLinkTag(CommandToken& token);

      /* レイヤ関連 */
      void MoveTag(CommandToken& token);
      void TransTag(CommandToken& token);

      /* MMD関連 */
      void CameraTag(CommandToken& token);

    };

  }
}
