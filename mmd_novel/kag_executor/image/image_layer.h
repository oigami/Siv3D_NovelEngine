#pragma once
#include <Siv3D.hpp>
namespace kag {
  class Layer {
    virtual void draw() const = 0;

  public:

    void Draw() const {
      draw();
    }
    void SetPositionLeft(int left) { pos_.x = left; }
    void SetPositionTop(int top) { pos_.y = top; }

    void IsVisible(bool visible) { visible_ = visible; }

  protected:

    Point pos_;
    bool visible_;
    int opacity_;
  };

  class ImageLayer : public Layer {
    void draw() const {
      if (!visible_) return;
      texture_.draw(pos_, Color(0xff, 0xff, 0xff, opacity_));
    }
  public:

    void SetTex(const Texture& tex) { texture_ = tex; }

  private:

    Texture texture_;
  };
}
