﻿#include <MmdNovel/layer.h>

namespace kag {
  LayerPage const kag::LayerPage::Fore(LayerPage::Type::Fore);
  LayerPage const kag::LayerPage::Back(LayerPage::Type::Back);
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
    if (trans_effect) {
      if (trans_effect->Update() == false)
        trans_effect.reset();
    }
    update();
  }

  void Layer::DrawPhase() const {
    if (trans_effect) {
      trans_effect->Draw();
    } else if (visible_) {
      draw();
    }
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

  void Layer::AddTrans(std::shared_ptr<ITransEffect> trans) {
    trans_effect = trans;
    trans->Update();
  }

  void LayerManagerImpl::Update() {
    for (auto& i : list_) {
      i.Update();
    }
  }
  void LayerManagerImpl::Draw() const {
    for (auto& i : list_) {
      i.Draw();
    }
  }
  namespace {
    auto comp = [](const PageLayer<LayerPtr>& a, const PageLayer<LayerPtr>& b) {
      return a[LayerPage::Fore] < b[LayerPage::Fore];
    };
  }
  void LayerManagerImpl::Set(const PageLayer<LayerPtr> & layer) {
    auto b = list_.begin();
    auto it = std::lower_bound(b, list_.end(), layer, comp);
    if (b != it) --it;
    list_.insert(it, layer);
  }
  void LayerManagerImpl::Remove(const PageLayer<LayerPtr> & layer) {
    auto e = list_.end();
    auto it = std::equal_range(list_.begin(), e, layer, comp);

    while (it.first < it.second) {
      if ((*it.first)[LayerPage::Fore] == layer[LayerPage::Fore]) {
        list_.erase(it.first);
        break;
      }
      ++it.first;
    }
  }

  void LayerManagerImpl::Update(const PageLayer<LayerPtr> & layer) {
    Remove(layer);
    Set(layer);
  }

  namespace detail {

    RenderTexture tex;

    void PageLayerTrans(int time_millisec, const LayerPtr & fore_layer, const LayerPtr & back_layer) {
      struct Trans : ITransEffect {
        Trans(int time_millisec, Layer* fore, Layer* back)
          :ITransEffect(time_millisec, fore, back) {
        }
        void update(int opacity) {
          fore_->SetOpacity(255 - opacity);
          back_->SetOpacity(opacity);
        }
        void draw()const {
          tex.clear(ColorF(0, 0, 0, 0));
          decltype(auto) def = Graphics2D::GetRenderTarget();
          Graphics2D::SetRenderTarget(tex);
          /* 表画面をそのまま出力 */
          Graphics2D::SetBlendState(BlendState::Opaque);
          fore_->draw();
          constexpr BlendState blend(true,
                                     /* カラーはアルファに応じて使用 */
                                     Blend::SrcAlpha, Blend::DestAlpha, BlendOp::Add,
                                     /* アルファは両方をそのまま使用 */
                                     Blend::One, Blend::One, BlendOp::Add);
          Graphics2D::SetBlendState(blend);

          back_->draw();
          Graphics2D::SetBlendState(BlendState::Default);
          Graphics2D::SetRenderTarget(Graphics::GetSwapChainTexture());
          Rect(0, 0, Window::Size())(tex).draw();
        }
      };
      decltype(auto) def = Graphics2D::GetRenderTarget();
      auto f = def.format;
      tex = RenderTexture{ Window::Size(),f };
      auto ptr = std::make_shared<Trans>(time_millisec, fore_layer.get(), back_layer.get());
      fore_layer->AddTrans(ptr);
    }

  }
}
