#pragma once
#include <queue>
#include <MmdNovel/kag_executor.h>

namespace kag
{
  class Executor::Pimpl
  {
    /// <summary>
    /// 内部でthisをキャプチャしてるのでクラスのコピー禁止
    /// <para>std::function&lt;void(Pimpl*)>とする手法があるがとりあえずこのまま</para>
    /// </summary>
    std::queue<std::function<void()>> command_;
    LayerManager layer_manager_;

    Pimpl(const Pimpl&) = delete;
    void operator=(const Pimpl&) = delete;

  public:

    Pimpl() {}

    LayerManager layerManager() { return layer_manager_; }

    template<class layer, class...Args> std::shared_ptr<typename type_traits::GetType<layer>::type> MakeLayer(Args&&...args)
    {
      return std::make_shared<typename type_traits::GetType<layer>::type>(layer_manager_, std::forward(args)...);
    }

    void Command(const std::function<void()>& f)
    {
      command_.push(f);
    }
  };
}
