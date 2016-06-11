#include <MmdNovel/message/message_manager.h>
#include "impl/message_manager_impl.h"

namespace kag {
  MessageManager::MessageManager() :pimpl_(std::make_shared<Pimpl>())
  {
  }

  void MessageManager::resize(size_t size)
  {
    pimpl_->resize(size);
  }

  void MessageManager::Clear()
  {
    pimpl_->Clear();
  }

  void MessageManager::AllClear()
  {
    pimpl_->AllClear();
  }

  bool MessageManager::IsFlush() const
  {
    return pimpl_->IsFlush();
  }

  void MessageManager::Flush()
  {
    pimpl_->Flush();
  }

  void MessageManager::Append(const SnapShotSpan & text)
  {
    pimpl_->Append(text);
  }

  void MessageManager::AppendNewLine()
  {
    pimpl_->AppendNewLine();
  }

  void MessageManager::NextPage()
  {
    pimpl_->NextPage();
  }

  void MessageManager::SetWaitClick() { pimpl_->SetWaitClick(); }

  void MessageManager::SetClickNextPage() { pimpl_->SetClickNextPage(); }

  bool MessageManager::Update()
  {
    return pimpl_->Update();
  }

  void MessageManager::Draw() const
  {
    pimpl_->Draw();
  }

  void MessageManager::SetCurrentLayer(int layer_index)
  {
    pimpl_->SetCurrentLayer(layer_index);
  }

  void MessageManager::SetCurrent(int layer_index, LayerPage type)
  {
    pimpl_->SetCurrent(layer_index, type);
  }

  void MessageManager::SetDelayTime(int delay_time)
  {
    pimpl_->SetDelayTime(delay_time);
  }

  void MessageManager::SetNoWaitText(bool is_no_wait)
  {
    pimpl_->SetNoWaitText(is_no_wait);
  }

  MessageLayer & MessageManager::GetLayer(int index, LayerPage page)
  {
    return pimpl_->GetLayer(index, page);
  }


  PageLayer<MessageLayer>& MessageManager::GetLayer(int index)
  {
    return pimpl_->GetLayer(index);
  }

  MessageLayer & MessageManager::Current() { return pimpl_->Current(); }

  int MessageManager::CurrentLayerNum() const { return pimpl_->CurrentLayerNum(); }

  LayerPage MessageManager::CurrentPage() const { return pimpl_->CurrentPage(); }

  void MessageManager::SetClickKey(const KeyCombination & key)
  {
    pimpl_->SetClickKey(key);
  }

  void MessageManager::SetInvalidKeyInput() { pimpl_->SetInvalidKeyInput(); }

  void MessageManager::SetValidKeyInput() { pimpl_->SetValidKeyInput(); }

  void MessageManager::SetLayerManager(LayerManager & manager)
  {
    pimpl_->SetLayerManager(manager);
  }

  bool MessageManager::IsWait() const { return pimpl_->IsWait(); }

}
