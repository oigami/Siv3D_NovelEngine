#include "layer.h"
namespace kag {
  MoveEffect::MoveEffect(Layer * layer, const Array<Data>& data)
    :layer_(layer), data_(data), now_easing_index_(0) {
    assert(layer);
  }

  MoveEffect::MoveEffect(Layer * layer, const Data & d)
    : MoveEffect(layer, Array<Data>(1, d)) {
  }

  Vec2 MoveEffect::GetMovePos(double t, const Data & d) const {
    return EasingType::func(d, elapsed(t, d.timeMillisec));
  }

  double MoveEffect::elapsed(double t, double timeMillisec) const {
    return Min<double>(t - pre_ease_time_, timeMillisec) / timeMillisec;
  }

  bool MoveEffect::update(double t) {
    t *= 1000;
    auto pos = GetMovePos(t, data_[now_easing_index_]);
    layer_->SetPositionLeft(pos.x);
    layer_->SetPositionTop(pos.y);
    if (t - pre_ease_time_ >= data_[now_easing_index_].timeMillisec) {
      now_easing_index_++;
      pre_ease_time_ = t;
      if (now_easing_index_ == data_.size()) {
        return false;
      }
    }
    return true;
  }

  double ScaleEffect::GetScale(double t, const Data & d) const {
    return EasingType::func(d, elapsed(t, d.timeMillisec));
  }

  double ScaleEffect::elapsed(double t, double timeMillisec) const {
    return Min<double>(t - pre_ease_time, timeMillisec) / timeMillisec;
  }

  ScaleEffect::ScaleEffect(Layer * layer, const Array<Data>& data)
    :layer_(layer), data_(data), now_easing_index_(0) {
    assert(layer);
  }

  bool ScaleEffect::update(double t) {
    t *= 1000;
    auto scale = GetScale(t, data_[now_easing_index_]);
    layer_->SetScale(scale);
    if (t - pre_ease_time >= data_[now_easing_index_].timeMillisec) {
      now_easing_index_++;
      pre_ease_time = t;
      if (now_easing_index_ == data_.size()) {
        return false;
      }
    }
    return true;
  }
}