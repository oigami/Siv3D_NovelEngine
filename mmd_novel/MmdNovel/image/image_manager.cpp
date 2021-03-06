#include "image_manager.h"
#define GET(name) get(L#name,name)
namespace kag
{
  namespace file
  {
    void ImageManager::AddTag(FuncList & list)
    {
      list[L"image"] = [this_ = shared_from_this()](CommandToken& token) { this_->ImageTag(token); };
    }

    ImageManager::ImageManager(const std::weak_ptr<Executor> & exe) :IFileManagerType(exe)
    {
      resize(2);
      for ( auto& i : step(2) )
      {
        const short index = static_cast<short>((i + 1) * 10);
        GetLayer(i, LayerPage::Fore)->SetZIndex(index);
        GetLayer(i, LayerPage::Back)->SetZIndex(index);
      }
    }

    void ImageManager::resize(int num)
    {
      static const ImageLayer fore = GetExecutor()->MakeLayer<ImageLayer>();
      IFileManager::resize(num, fore, fore);
    }

    void ImageManager::ImageTag(CommandToken & token)
    {
      ImageVal tag(shared_from_this(), token);
      if ( token.HasError() ) return;
      GetExecutor()->Command([tag = std::move(tag)]() { tag.attach(); });
    }

    ImageManager::ImageVal::ImageVal(const std::shared_ptr<ImageManager>& manager, CommandToken & token) :manager_(manager)
    {
      token.GET(layer).GET(page).GET(storage).GET(left).GET(top)
        .GET(opacity).GET(index).GET(visible);
    }

    ImageManager::ImageVal::ImageVal(const std::shared_ptr<ImageManager>& manager, int layer_index, const SnapShotSpan & storage)
      :manager_(manager), layer({ L"image", layer_index }), storage(storage)
    {
    }

    void ImageManager::ImageVal::attach() const
    {
      auto ptr = manager_->GetLayer(layer->second, page);
      if ( storage->Length() == 0 )
      {
        ptr->SetTex();
      }
      else
      {
        ptr->SetTex(Texture(storage->ToStr()));
      }
      if ( left ) ptr->SetPositionLeft(*left);
      if ( top ) ptr->SetPositionLeft(*top);
      if ( index ) ptr->SetZIndex(*index);
      if ( visible ) ptr->IsVisible(*visible);
    }

  }
}
