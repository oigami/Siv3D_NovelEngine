#include <MmdNovel/layer.h>

namespace kag {

  namespace {

    struct MoveEffect : IEffect {
      using Data = MoveEffectData;
      using DataArray = Array<Data>;
      MoveEffect(Layer* layer, const Array<Data>& data);
      MoveEffect(Layer* layer, const Data& d);
    private:
      Array<Data> data_;
      Layer* layer_;
      int now_easing_index_;
      double pre_ease_time_ = 0;

      Vec2 GetMovePos(double t, const Data& d) const;
      double elapsed(double t, double timeMillisec) const;
      bool update(double t) override;
    };

    struct ScaleEffect : IEffect {
      using Data = ScaleEffectData;
      ScaleEffect(Layer* layer, const Array<Data>& data);

    private:
      double GetScale(double t, const Data& d) const;
      double elapsed(double t, double timeMillisec) const;

      Array<Data> data_;
      // 親のメモリとEffectの寿命が同じなので生ポインタにする
      // しないとプログラム終了時にSiv3DがEffectを強制解放した時に2重解放が起きてエラーが出る
      Layer* layer_;
      int now_easing_index_;
      double pre_ease_time = 0;

      bool update(double t) override;
    };

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
      auto pos = GetMovePos(t, data_[now_easing_index_]).asPoint();
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

  Layer::Layer() :opacity_(255), visible_(true), z_index_(0) {}

  void Layer::Update() {
    effect.update();
    update();
  }

  void Layer::Draw() const {
    if (visible_) draw();
  }

  void Layer::SetOpacity(int opacity) { opacity_ = static_cast<uint8>(opacity); }
  void Layer::SetPositionLeft(int left) { position_.x = left; }
  void Layer::SetPositionTop(int top) { position_.y = top; }
  void Layer::SetPositionWidth(int width) { normal_size_.x = position_.w = width; }
  void Layer::SetPositionHeight(int height) { normal_size_.y = position_.h = height; }
  void Layer::SetScale(double s) {
    position_.w = static_cast<int>(normal_size_.x * s);
    position_.h = static_cast<int>(normal_size_.y * s);
  }
  void Layer::IsVisible(bool visible) { visible_ = visible; }
  void Layer::SetZIndex(uint16 index) { z_index_ = index; }
  void Layer::MoveEffect(const MoveEffectData & data) { effect.add<kag::MoveEffect>(this, data); }
  void Layer::MoveEffect(const MoveEffectData::Array & data) { effect.add<kag::MoveEffect>(this, data); }
  void Layer::ScaleEffect(const ScaleEffectData& data) { ScaleEffect(Array<ScaleEffect::Data>(1, data)); }
  void Layer::ScaleEffect(const ScaleEffectData::Array& data) { effect.add<kag::ScaleEffect>(this, data); }

  void LayerManagerImpl::Update() {
    for (auto& i : list_) {
      i->Update();
    }
  }
  void LayerManagerImpl::Draw() const {
    for (auto& i : list_) {
      i->Draw();
    }
  }
  void LayerManagerImpl::Set(const LayerPtr & layer) {
    auto b = list_.begin();
    auto it = std::lower_bound(b, list_.end(), layer);
    if (b != it) --it;
    list_.insert(it, layer);
  }
  void LayerManagerImpl::Remove(const LayerPtr & layer) {
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
  void LayerManagerImpl::Update(const LayerPtr & layer) {
    Remove(layer);
    Set(layer);
  }
}
