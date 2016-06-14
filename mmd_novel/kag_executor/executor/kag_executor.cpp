#include <MmdNovel/kag_executor.h>
#include <MmdNovel/tag_editor.h>
#include <kag_executor/executor/impl/kag_executor_impl.h>
#include <MmdNovel/imanager.h>
namespace kag
{
  Executor::Executor() :pimpl_(std::make_shared<Pimpl>()) {}

  PageLayer<LayerPtr> Executor::GetLayer(std::pair<kag::converter::LayerType, int> layer_num) const
  {
    using namespace converter;
    switch ( layer_num.first )
    {
    case LayerType::Message:
      return messageManager().GetLayer(layer_num.second);
    case LayerType::Background:
      assert(0);
      throw std::runtime_error("Executor::Getlayer() Background");
    case LayerType::Foreground:
      return imageManager()->GetLayer(layer_num.second);
    case LayerType::MMD:
      throw std::runtime_error("Executor::Getlayer() mmd");
    default:
      throw std::runtime_error("Executor::Getlayer() other");
      break;
    }

  }
  MessageManager Executor::messageManager() const
  {
    return pimpl_->messageManager();
  }
  ImageManager Executor::imageManager() const
  {
    return pimpl_->imageManager();
  }

  void Executor::Clear()
  {
    pimpl_->Clear();
  }

  bool Executor::IsWait() const
  {
    return pimpl_->IsWait();
  }

  void Executor::Command(const std::function<void()>& f)
  {
    pimpl_->Command(f);
  }

  void Executor::CommandL()
  {
    pimpl_->CommandL();
  }

  void Executor::CommandR()
  {
    pimpl_->CommandR();
  }

  void Executor::CommandP()
  {
    pimpl_->CommandP();
  }

  void Executor::CommandDelay(int delay_time)
  {
    pimpl_->CommandDelay(delay_time);
  }

  void Executor::CommandNoWait()
  {
    pimpl_->CommandNoWait();
  }

  void Executor::CommandEndNoWait()
  {
    pimpl_->CommandEndNoWait();
  }

  void Executor::CommandER()
  {
    pimpl_->CommandER();
  }

  void Executor::CommandCM()
  {
    pimpl_->CommandCM();
  }

  void Executor::CommandCT()
  {
    pimpl_->CommandCT();
  }

  void Executor::CommandText(const SnapShotSpan& str)
  {
    pimpl_->CommandText(str);
  }

  void Executor::CommandTextNoDelay(const SnapShotSpan & str)
  {
    pimpl_->CommandTextNoDelay(str);
  }

  void Executor::CommandCurrent(int index, const LayerPage& page)
  {
    pimpl_->CommandCurrent(index, page);
  }

  void Executor::CommandIndent()
  {
    pimpl_->CommandIndent();
  }

  void Executor::CommandEndIndent()
  {
    pimpl_->CommandEndIndent();
  }

  void Executor::CommandLocate(Value<int> x, Value<int> y)
  {
    pimpl_->CommandLocate(x, y);
  }

  void Executor::CommandStyle(const CommandFunc<StyleCommandEditor>& f)
  {
    pimpl_->CommandStyle(f);
  }

  void Executor::CommandDefStyle(const CommandFunc<DefaultStyleCommandEditor>& f)
  {
    pimpl_->CommandDefStyle(f);
  }

  void Executor::CommandResetStyle()
  {
    pimpl_->CommandResetStyle();
  }

  void Executor::CommandFont(const CommandFunc<FontCommandEditor>& f)
  {
    pimpl_->CommandFont(f);
  }

  void Executor::CommandDefFont(const CommandFunc<DefFontCommandEditor>& f)
  {
    pimpl_->CommandDefFont(f);
  }

  void Executor::CommandResetFont()
  {
    pimpl_->CommandResetFont();
  }

  void Executor::CommandPosition(Value<int> layer, const Value<LayerPage>& page, const CommandFunc<PositionCommandEditor>& f)
  {
    pimpl_->CommandPosition(layer, page, f);
  }

  void Executor::CommandImage(int layer, const Value<LayerPage>& page, const Texture & tex)
  {
    pimpl_->CommandImage(layer, page, tex);
  }

  void Executor::AddManager(const SnapShotSpan & name, const IManagerPtr & manager)
  {
    manager_[name] = manager;
  }

  void Executor::ShowErrorMsg(const String & str) const
  {
    MessageBox::Show(str);
  }

  bool Executor::Update()
  {
    return pimpl_->Update();
  }

  bool Executor::CommandUpdate()
  {
    return pimpl_->CommandUpdate();
  }

  void Executor::Draw()
  {
    pimpl_->Draw();
  }


}
