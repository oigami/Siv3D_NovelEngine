#include <MmdNovel/live2d/live2d_manager.h>
#define GET(name) get(L#name,name)

void kag::live2d::Live2DManager::AddTag(FuncList & list)
{
  list[L"live2d"] = Bind(&Live2DManager::Live2DTag);
}

void kag::live2d::Live2DManager::update()
{
}

kag::live2d::Live2DManager::Live2DManager(const std::weak_ptr<Executor>& exe) :IFileManagerType(exe)
{
  IManager::resize(1, exe.lock()->MakeLayer<Live2DLayerPimpl>(), exe.lock()->MakeLayer<Live2DLayerPimpl>());
}

void kag::live2d::Live2DManager::Live2DTag(kag::file::CommandToken & token)
{
  kag::file::Must<SnapShotSpan> storage, texture;
  if ( token.GET(storage).GET(texture).HasError() ) return;

  auto res = live2d_.LoadModel(storage->ToStr(), { texture->ToStr() });
  auto layer = GetLayer<Live2DLayerPimpl>(0, LayerPage::Fore);
  layer->model = res;
}
