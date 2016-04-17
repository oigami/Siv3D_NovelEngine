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
    void SetPositionLeft(int left) { pos_.x = left; }
    void SetPositionTop(int top) { pos_.y = top; }

    void IsVisible(bool visible) { visible_ = visible; }

    void SetZIndex(uint16 index) { z_index_ = index; }

    bool operator<(const Layer& layer) const { return z_index_ < layer.z_index_; }

  protected:

    Point pos_;
    bool visible_;
    int opacity_;
    uint16 z_index_;
  };

  class ImageLayer : public Layer {
    void draw() const {
      //TODO
      //if (!visible_) return;
      if (!texture_) return;
      texture_.draw(pos_, Color(0xff, 0xff, 0xff, opacity_));
    }
  public:

    void SetTex(const Texture& tex) { texture_ = tex; }

  private:

    Texture texture_;
  };
}
