#pragma once
#include <Siv3D.hpp>
namespace kag {
  class Layer {
    virtual void draw() const = 0;

  public:
    Layer() :opacity_(255), visible_(false), z_index_(0) {}

    void Draw() const {
      draw();
    }

    void SetPositionLeft(int left) { position_.x = left; }
    void SetPositionTop(int top) { position_.y = top; }
    void SetPositionWidth(int width) { position_.w = width; }
    void SetPositionHeight(int height) { position_.h = height; }

    void IsVisible(bool visible) { visible_ = visible; }

    void SetZIndex(uint16 index) { z_index_ = index; }

    bool operator<(const Layer& layer) const { return z_index_ < layer.z_index_; }
    bool operator<=(const Layer& layer) const { return z_index_ <= layer.z_index_; }

  protected:

    Rect position_;
    uint16 z_index_;
    uint8 opacity_;
    bool visible_;
  };
  using LayerPtr = std::shared_ptr<Layer>;

  class LayerManagerImpl {
    Array<LayerPtr> list_;
  private:
    class DummyLayer : public Layer {
      virtual void draw() {}
    };

  public:

    void Draw() const {
      for (auto& i : list_) {
        i->Draw();
      }
    }

    void Set(const LayerPtr& layer) {
      auto b = list_.begin();
      auto it = std::lower_bound(b, list_.end(), layer);
      if (b != it) --it;
      list_.insert(it, layer);
    }

    void Remove(const LayerPtr& layer) {
      auto e = list_.end();
      auto it = std::equal_range(list_.begin(), e, layer);

      while (it.first < it.second) {
        if (*it.first == layer) {
          list_.erase(it.first);
          break;
        }
        ++it.first;
      }
    }

    void Update(const LayerPtr& layer) {
      Remove(layer);
      Set(layer);
    }
  };

  using LayerManager = std::shared_ptr<LayerManagerImpl>;
}
