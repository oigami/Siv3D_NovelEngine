#pragma once
#include <memory>
#include <Siv3D.hpp>
#include <MmdNovel/default_value.h>
namespace kag
{
  struct Layer;
  using LayerPtr = std::shared_ptr<Layer>;

  struct EasingType
  {
    enum class Type
    {
      In, Out, InOut,
    };
    using EaseFunc = double(*)(double t);

    template<class T> struct Data
    {
      using Array = s3d::Array<Data<T>>;
      Data() = default;
      Data(T s, T e, EaseFunc f, Type t, int time)
        :start(s), end(e), ease_func(f), type(t), timeMillisec(time)
      {
      }

      T start, end;
      EaseFunc ease_func = Easing::Linear;
      EasingType::Type type = EasingType::Type::In;
      int timeMillisec = 1000;
    };

    template<class T>
    static T func(Type type, T start, T end, std::function<double(double)> f, double now_timeMillisec)
    {
      using EaseInOutFunc = T(*)(const T& s, const T& e, std::function<double(double)>, double t);

      static const EaseInOutFunc func[3] = {
        (EaseInOutFunc) EaseIn<T>,
        (EaseInOutFunc) EaseOut<T>,
        (EaseInOutFunc) EaseInOut<T>
      };
      int t = static_cast<int>(type);
      return func[t](start, end, f, now_timeMillisec);
    }

    template<class T>
    static T func(const Data<T>& d, double now_timeMillisec)
    {
      return func(d.type, d.start, d.end, d.ease_func, now_timeMillisec);
    }
  };
  using MoveEffectData = EasingType::Data<Vec2>;

  using ScaleEffectData = EasingType::Data<double>;
  class ITransEffect;

  struct TransUniversalData
  {
    int time_millisec;

    // ルール画像（グレースケール)
    Texture rule_tex;

    // あいまい領域値
    int vague;
  };

  struct TransScrollData
  {
    enum class Stay
    {
      StayFore, StayBack, NoStay
    };
    enum class From
    {
      Left, Top, Right, Bottom
    };

    int time_millisec;

    // 裏ページが現れる方向
    From from;

    // 表ページの挙動
    Stay stay;

  };

  class LayerManagerImpl;
  using LayerManager = std::shared_ptr<LayerManagerImpl>;

  class ITransEffect
  {
    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="t"> 現在の裏画面の不透明度 </param>
    virtual void update(double opacity) = 0;

    virtual void draw()const = 0;

  public:

    ITransEffect(int time_millisec, Layer* fore, Layer* back);

    bool Update();

    void Draw() const;

  protected:

    Layer *fore_, *back_;

  private:
    EasingController<double> t;

  };
  namespace detail
  {
    void PageLayerTrans(int time_millisec, const LayerPtr& fore_layer, const LayerPtr& back_layer);
    void PageLayerTrans(const TransUniversalData& data, const LayerPtr& fore_layer, const LayerPtr& back_layer);
  }

  class LayerPage
  {
    friend Value<LayerPage>;
    constexpr LayerPage(detail::Default) :page(-1)
    {
    }
  public:

    enum class Type
    {
      Fore,
      Back
    };
    constexpr LayerPage() :page(Define::fore_page) {}

    constexpr LayerPage(Type type) : page(type == Type::Fore ? Define::fore_page : Define::back_page)
    {
    }

    constexpr operator int() const { return page; }
    int page;
    static const LayerPage Fore;
    static const LayerPage Back;
  };

  template<class Pimpl> class PageLayer
  {
  public:
    PageLayer(std::array<LayerPtr, 2>& l) : layer_(l) {}
    PageLayer() = default;
    PageLayer(const Pimpl& fore, const Pimpl& back) : layer_({ fore,back }) {}

    Pimpl& operator[](const LayerPage& page)
    {
      return layer_[page];
    }
    const Pimpl& operator[](const LayerPage& page) const
    {
      return layer_[page];
    }
    const Pimpl& Fore() const
    {
      return layer_[LayerPage::Fore];
    }

    const Pimpl& Back() const
    {
      return layer_[LayerPage::Back];
    }

    operator PageLayer<LayerPtr>()const
    {
      std::array<LayerPtr, 2> ret;
      for ( auto& i : step(2) )
      {
        ret[i] = layer_[i];
      }
      PageLayer<LayerPtr> tmp(ret);
      return tmp;
    }

    void Update()
    {
      layer_[LayerPage::Fore]->Update();
    }

    void Draw()const
    {
      layer_[LayerPage::Fore]->DrawPhase();
    }

    void Trans(int time_millisec)
    {
      detail::PageLayerTrans(time_millisec, layer_[LayerPage::Fore], layer_[LayerPage::Back]);
    }
    void Trans(const TransUniversalData& data)
    {
      detail::PageLayerTrans(data, layer_[LayerPage::Fore], layer_[LayerPage::Back]);
    }
    void Trans(const TransScrollData& data);

  private:


    std::array<Pimpl, 2> layer_;
  };



}
