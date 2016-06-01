#include "layer.h"
namespace kag {
  bool MoveEffect::update(double t) {
    auto pos = data_.easing[now_easing_index_].easeIn();
    layer_->SetPositionLeft(pos.x);
    layer_->SetPositionTop(pos.y);
    if (data_.easing[now_easing_index_].isEnd()) {
      now_easing_index_++;
      if (now_easing_index_ != data_.easing.size()) {
        data_.easing[now_easing_index_].start();
      } else {
        return false;
      }
    }
    return true;
  }
  bool ScaleEffect::update(double t) {
    auto scale = data_.easing[now_easing_index_].easeIn();
    layer_->SetScale(scale);
    if (data_.easing[now_easing_index_].isEnd()) {
      now_easing_index_++;
      if (now_easing_index_ != data_.easing.size()) {
        data_.easing[now_easing_index_].start();
      } else {
        return false;
      }
    }
    return true;
  }
}