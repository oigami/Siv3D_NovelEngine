#include <MmdNovel/layer/layer_manager.h>
#include <MmdNovel/layer/ilayer.h>
namespace kag
{

  void LayerManagerImpl::Update()
  {
    for ( auto& i : list_ )
    {
      i.Update();
    }
  }
  void LayerManagerImpl::Draw() const
  {
    for ( auto& i : list_ )
    {
      i.Draw();
    }
  }
  namespace
  {
    auto comp = [](const auto& a, const auto& b)
    {
      return *a[LayerPage::Fore] < *b[LayerPage::Fore];
    };
  }
  void LayerManagerImpl::Set(const PageLayer<LayerPtr> & layer)
  {
    auto b = list_.begin();
    auto it = std::lower_bound(b, list_.end(), layer, comp);
    if ( b != it ) --it;
    list_.insert(it, PageLayer<ILayer*>(layer.Fore().get(), layer.Back().get()));
  }
  void LayerManagerImpl::Remove(const PageLayer<LayerPtr> & layer)
  {
    auto e = list_.end();
    auto it = std::equal_range(list_.begin(), e, layer, comp);

    while ( it.first < it.second )
    {
      if ( (*it.first)[LayerPage::Fore] == layer[LayerPage::Fore].get() )
      {
        list_.erase(it.first);
        break;
      }
      ++it.first;
    }
  }

  void LayerManagerImpl::Update(const PageLayer<LayerPtr> & layer)
  {
    Remove(layer);
    Set(layer);
  }

  void LayerManagerImpl::Sort()
  {
    std::sort(list_.begin(), list_.end(), comp);
  }

}
