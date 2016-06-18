#pragma once
#include <Siv3D.hpp>
#include <MmdNovel/default_value.h>
#include <MmdNovel/type_traits.h>
#include <MmdNovel/kag_executor.h>
namespace kag
{
  struct ILayer;
  using LayerPtr = std::shared_ptr<ILayer>;


  struct ILayer
  {
  private:
    virtual void update() {};
  public:
    virtual void clone(LayerPtr& out) = 0;
    template<class LayerType>
    static void DefaultClone(LayerType& this_, LayerPtr& out)
    {
      auto out_c = std::dynamic_pointer_cast<LayerType>(out);
      if ( out_c )
      {
        *out_c = this_;
      }
      else
      {
        out = std::make_shared<LayerType>(this_);
      }
    }

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <remarks>トランジションはこちらのdrawを呼ぶ</remarks>
    virtual void draw() const = 0;

    virtual ~ILayer() = default;

    ILayer(const LayerManager& manager);

    void Update();

    void DrawPhase() const;
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

    bool operator<(const ILayer& layer) const { return z_index_ < layer.z_index_; }
    bool operator<=(const ILayer& layer) const { return z_index_ <= layer.z_index_; }

    const Rect& position()const { return position_; }
    const uint8& opacity()const { return opacity_; }

    void AddTrans(std::shared_ptr<ITransEffect> trans);

  private:

    // トランジション用のエフェクト
    std::shared_ptr<ITransEffect> trans_effect;
    LayerManager layer_manager_;
    Effect effect;
    Size normal_size_;
    Rect position_;
    uint16 z_index_;
    uint8 opacity_;
    bool visible_;
  };






}
