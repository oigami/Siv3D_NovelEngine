#pragma once
#include <Siv3D.hpp>
namespace kag {
  class Layer;
  using LayerPtr = std::shared_ptr<Layer>;

  struct EasingType {
    enum class Type {
      In, Out, InOut,
    };
    template<class T>
    using EaseInOutFunc = T(*)(const T& s, const T& e, std::function<double(double)>, double t);
    using EaseFunc = double(*)(double t);

    template<class T>
    struct Data {
      Data() = default;
      Data(T s, T e, EaseFunc f, Type t, int time)
        :start(s), end(e), ease_func(f), type(t), timeMillisec(time) {
      }

      T start, end;
      EaseFunc ease_func = Easing::Linear;
      EasingType::Type type = EasingType::Type::In;
      int timeMillisec = 1000;
    };

    template<class T>
    static T func(Type type, T start, T end, std::function<double(double)> f, double now_timeMillisec) {
      static const EaseInOutFunc<T> func[3] = {
        (EaseInOutFunc<T>)EaseIn<T>,
        (EaseInOutFunc<T>)EaseOut<T>,
        (EaseInOutFunc<T>)EaseInOut<T>
      };
      int t = static_cast<int>(type);
      return func[t](start, end, f, now_timeMillisec);
    }

    template<class T>
    static T func(const Data<T>& d, double now_timeMillisec) {
      return func(d.type, d.start, d.end, d.ease_func, now_timeMillisec);
    }
  };

  struct MoveEffect : IEffect {
    using Data = EasingType::Data<Vec2>;
    using DataArray = Array<Data>;
    MoveEffect(Layer* layer, const Array<Data>& data);
    MoveEffect(Layer* layer, const Data& d);
  private:
    Array<Data> data_;
    Layer* layer_;
    int now_easing_index_;
    int pre_ease_time_ = 0;

    Vec2 GetMovePos(double t, const Data& d) const;
    double elapsed(double t, double timeMillisec) const;
    bool update(double t) override;
  };

  struct ScaleEffect : IEffect {
    using Data = EasingType::Data<double>;
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

    void MoveEffect(const MoveEffect::Data& data) { effect.add<kag::MoveEffect>(this, data); }
    void MoveEffect(const MoveEffect::DataArray& data) { effect.add<kag::MoveEffect>(this, data); }

    void ScaleEffect(ScaleEffect::Data data) { ScaleEffect(Array<ScaleEffect::Data>(1, data)); }
    void ScaleEffect(const Array<ScaleEffect::Data>& data) { effect.add<kag::ScaleEffect>(this, data); }

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
