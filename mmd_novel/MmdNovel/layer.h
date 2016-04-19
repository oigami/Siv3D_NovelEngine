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

  protected:

    Rect position_;
    bool visible_;
    int opacity_;
    uint16 z_index_;
  };
}
