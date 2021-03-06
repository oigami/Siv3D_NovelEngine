#pragma once
#include <MmdNovel/image/image_layer.h>
#include <MmdNovel/default_value.h>
#include <MmdNovel/kag_file_executor.h>
namespace kag
{
  namespace file
  {
    struct ImageManager : public IFileManagerType<ImageManager>
    {
    private:
      virtual void AddTag(FuncList& list)override;
      virtual void update() {}
    public:
      ImageManager(const std::weak_ptr<Executor>& exe);

      void resize(int num);

      ImageLayer GetLayer(int layer, LayerPage page) { return IFileManager::GetLayer<ImageLayer>(layer, page); }
      struct ImageVal
      {
        Must<std::pair<LayerType, int>> layer;
        LayerPage page = LayerPage::Fore;
        Must<SnapShotSpan> storage;
        Optional<int> left, top;
        Optional<int> opacity;
        Optional<int> index;
        Optional<bool> visible;

        std::shared_ptr<ImageManager> manager_;
        ImageVal(const std::shared_ptr<ImageManager>& manager, CommandToken& token);
        ImageVal(const std::shared_ptr<ImageManager>& manager, int layer_index, const SnapShotSpan& storage);

        void attach() const;
      };

      void ImageTag(CommandToken & token);
    private:

    };
  }
}
