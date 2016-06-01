﻿#pragma once
#include <Siv3D.hpp>
namespace kag {
  class Layer;
  using LayerPtr = std::shared_ptr<Layer>;

  struct EasingType {
    enum class Type {
      In, Out, InOut,
    };
    using EaseFunc = double(*)(double t);

    template<class T> struct Data {
      using Array = s3d::Array<Data<T>>;
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
      using EaseInOutFunc = T(*)(const T& s, const T& e, std::function<double(double)>, double t);

      static const EaseInOutFunc func[3] = {
        (EaseInOutFunc)EaseIn<T>,
        (EaseInOutFunc)EaseOut<T>,
        (EaseInOutFunc)EaseInOut<T>
      };
      int t = static_cast<int>(type);
      return func[t](start, end, f, now_timeMillisec);
    }

    template<class T>
    static T func(const Data<T>& d, double now_timeMillisec) {
      return func(d.type, d.start, d.end, d.ease_func, now_timeMillisec);
    }
  };
  using MoveEffectData = EasingType::Data<Vec2>;

  using ScaleEffectData = EasingType::Data<double>;

  class Layer : std::enable_shared_from_this<Layer> {
    virtual void draw() const = 0;
    virtual void update() {};

  public:
    virtual ~Layer() = default;
    Layer();

    void Update();

    void Draw() const;
    void SetOpacity(int opacity);
    void SetPositionLeft(int left);
    void SetPositionTop(int top);
    void SetPositionWidth(int width);
    void SetPositionHeight(int height);

    void SetScale(double s);

    void IsVisible(bool visible);

    void SetZIndex(uint16 index);

    void MoveEffect(const MoveEffectData& data);
    void MoveEffect(const MoveEffectData::Array& data);

    void ScaleEffect(const ScaleEffectData& data);
    void ScaleEffect(const ScaleEffectData::Array& data);

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
