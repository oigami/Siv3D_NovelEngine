#pragma once
#include <MmdNovel/image/image_layer.h>
#include <MmdNovel/default_value.h>
namespace kag {
  template<class Impl> class ManagerHelper {
  public:
    ManagerHelper() :manager_(Impl::create()) {}

    std::shared_ptr<Impl>& operator->() { return manager_; }

  private:

    std::shared_ptr<Impl> manager_;
  };
  namespace pimpl {
    class ImageManagerImpl {
    public:
      static std::shared_ptr<ImageManagerImpl> create() {
        return std::make_shared<ImageManagerImpl>();
      }
      ImageManagerImpl() :layer_(2) {
        for (auto& i : step(layer_.size())) {
          const short index = static_cast<short>((i + 1) * 10);
          layer_[i][LayerPage::Fore]->SetZIndex(index);
          layer_[i][LayerPage::Back]->SetZIndex(index);
        }
      }

      ImageLayer& GetLayer(int layer, LayerPage page) { return layer_[layer][page]; }
      PageLayer<ImageLayer>& GetLayer(int layer) { return layer_[layer]; }
      void SetLayerManager(LayerManager& manager) {
        layer_manager_ = manager;
        for (auto& i : layer_) {
          layer_manager_->Set(i);
        }
      }
    private:

      LayerManager layer_manager_;
      Array<PageLayer<ImageLayer>> layer_;
    };
  }
  using ImageManager = ManagerHelper<pimpl::ImageManagerImpl>;
}
