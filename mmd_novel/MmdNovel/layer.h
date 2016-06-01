#pragma once
#include <Siv3D.hpp>
namespace kag {
  class Layer;
  using LayerPtr = std::shared_ptr<Layer>;

  struct MoveEffect : IEffect {
    struct Data {
      Array<EasingController<Vec2>> easing;
    };

    Data data_;
    Layer* layer_;
    int now_easing_index_;
    MoveEffect(Layer* layer, Data data)
      :layer_(layer), data_(data), now_easing_index_(0) {
      assert(layer);
      data_.easing[0].start();
    }

    bool update(double t) override;
  };

  struct ScaleEffect : IEffect {
    struct Data {
      Array<EasingController<double>> easing;
    };

    Data data_;
    // 親のメモリとEffectの寿命が同じなので生ポインタにする
    // しないとプログラム終了時にSiv3DがEffectを強制解放した時に2重解放が起きてエラーが出る
    Layer* layer_;
    int now_easing_index_;
    int pre_scale = 1;
    ScaleEffect(Layer* layer, Data data)
      :layer_(layer), data_(data), now_easing_index_(0) {
      assert(layer);
      data_.easing[0].start();
    }

    bool update(double t) override;
  };

  class Layer : std::enable_shared_from_this<Layer> {
    virtual void draw() const = 0;
    virtual void update() {};

  public:
    virtual ~Layer() = default;
    Layer() :opacity_(255), visible_(true), z_index_(0) {}

    void Update() {
      effect.update();
      update();
    }

    void Draw() const {
      if (visible_) draw();
    }
    void SetOpacity(int opacity) { opacity_ = static_cast<uint8>(opacity); }
    void SetPositionLeft(int left) { position_.x = left; }
    void SetPositionTop(int top) { position_.y = top; }
    void SetPositionWidth(int width) { normal_size_.x = position_.w = width; }
    void SetPositionHeight(int height) { normal_size_.y = position_.h = height; }

    void SetScale(double s) {
      position_.w = static_cast<int>(normal_size_.x * s);
      position_.h = static_cast<int>(normal_size_.y * s);
    }

    void IsVisible(bool visible) { visible_ = visible; }

    void SetZIndex(uint16 index) { z_index_ = index; }

    void MoveEffect(MoveEffect::Data data) { effect.add<kag::MoveEffect>(this, data); }
    void ScaleEffect(ScaleEffect::Data data) { effect.add<kag::ScaleEffect>(this, data); }

    bool operator<(const Layer& layer) const { return z_index_ < layer.z_index_; }
    bool operator<=(const Layer& layer) const { return z_index_ <= layer.z_index_; }


    const Rect& position()const { return position_; }
    const uint8& opacity()const { return opacity_; }

  private:
    Effect effect;
    Size normal_size_;
    Rect position_;
    uint16 z_index_;
    uint8 opacity_;
    bool visible_;
  };


  template<class Pimpl> class LayerHelper {
  public:
    LayerHelper() : pimpl_(Pimpl::create()) {}
    operator LayerPtr() const { return pimpl_; }
    std::shared_ptr<Pimpl> operator->() const { return pimpl_; }

    Pimpl& operator()() const;

    template<class T, class ...Args>
    auto bind(T(Layer::* f)(Args...)) {
      return [this, f](Args...args) { return (((LayerPtr&)pimpl_).get()->*f)(args...); };
    }

  private:

    std::shared_ptr<Pimpl> pimpl_;
  };

  class LayerManagerImpl {
    Array<LayerPtr> list_;
  private:
    class DummyLayer : public Layer {
      virtual void draw() {}
    };

  public:
    void Update() {
      for (auto& i : list_) {
        i->Update();
      }
    }

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
