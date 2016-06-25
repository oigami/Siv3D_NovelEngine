#include <MmdNovel/layer/page_layer.h>
#include <MmdNovel/layer/ilayer.h>

namespace kag
{
  ILayerPtr& PageLayer::operator[](const LayerPage& page)
  {
    return layer_[page];
  }

  const ILayerPtr& PageLayer::operator[](const LayerPage& page) const
  {
    return layer_[page];
  }

  const ILayerPtr & PageLayer::Fore() const
  {
    return layer_[LayerPage::Fore];
  }

  ILayerPtr & PageLayer::Fore()
  {
    return layer_[LayerPage::Fore];
  }

  const ILayerPtr & PageLayer::Back() const
  {
    return layer_[LayerPage::Back];
  }

  ILayerPtr & PageLayer::Back()
  {
    return layer_[LayerPage::Back];
  }

  void PageLayer::Update()
  {
    layer_[LayerPage::Fore]->Update();
  }

  void PageLayer::Draw() const
  {
    layer_[LayerPage::Fore]->DrawPhase();
  }

  void PageLayer::Trans(int time_millisec)
  {
    detail::PageLayerTrans(time_millisec, layer_[LayerPage::Fore], layer_[LayerPage::Back]);
  }

  void PageLayer::Trans(const TransUniversalData & data)
  {
    detail::PageLayerTrans(data, layer_[LayerPage::Fore], layer_[LayerPage::Back]);
  }
}
