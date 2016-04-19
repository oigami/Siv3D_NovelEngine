#include "impl/message_layer_impl.h"
namespace kag {

  MessageLayer::MessageLayer() :pimpl_(std::make_shared<Pimpl>()) {

  }

  void MessageLayer::Clear() {
    pimpl_->Clear();
  }

  void MessageLayer::NextPage() {
    pimpl_->NextPage();
  }

  void MessageLayer::Append(const String & str) {
    pimpl_->Append(str);
  }

  void MessageLayer::Append(const wchar & str) {
    pimpl_->Append(str);
  }

  bool MessageLayer::IsLimitHeihgt() {
    return pimpl_->IsLimitHeihgt();
  }

  void MessageLayer::AppenNewLine() {
    pimpl_->AppenNewLine();
  }

  void MessageLayer::Draw() const {
    pimpl_->Draw();
  }

  const message::TextFont & MessageLayer::NowFont() const { return pimpl_->NowFont(); }

  void MessageLayer::SetFont(const message::TextFont & font) {
    pimpl_->SetFont(font);
  }

  void MessageLayer::SetDefaultFont(const message::TextFont & font) {
    pimpl_->SetDefaultFont(font);
  }

  void MessageLayer::ResetFont() {
    pimpl_->ResetFont();
  }

  void MessageLayer::SetPositionTop(int top) {
    pimpl_->SetPositionTop(top);
  }

  void MessageLayer::SetPositionLeft(int left) {
    pimpl_->SetPositionLeft(left);
  }

  void MessageLayer::SetPositionWidth(int width) {
    pimpl_->SetPositionWidth(width);
  }

  void MessageLayer::SetPositionHeight(int height) {
    pimpl_->SetPositionHeight(height);
  }

  void MessageLayer::SetMarginTop(int top) {
    pimpl_->SetMarginTop(top);
  }

  void MessageLayer::SetMarginLeft(int left) {
    pimpl_->SetMarginLeft(left);
  }

  void MessageLayer::SetMarginRight(int width) {
    pimpl_->SetMarginRight(width);
  }

  void MessageLayer::SetMarginBottom(int height) {
    pimpl_->SetMarginBottom(height);
  }

  void MessageLayer::SetVisible(bool is_visible) {
    pimpl_->SetVisible(is_visible);
  }

  void MessageLayer::SetBackgroundColor(Color argb) {
    pimpl_->SetBackgroundColor(argb);
  }

  void MessageLayer::SetBackgroundRGB(int r, int g, int b) {
    pimpl_->SetBackgroundRGB(r, g, b);
  }

  void MessageLayer::SetBackgroundOpacity(int a) {
    pimpl_->SetBackgroundOpacity(a);
  }

  void MessageLayer::SetBackgroundTex(Texture tex) {
    pimpl_->SetBackgroundTex(tex);
  }

  void MessageLayer::SetLocate(int x, int y) {
    pimpl_->SetLocate(x, y);
  }

  void MessageLayer::SetLocateX(int x) {
    pimpl_->SetLocateX(x);
  }

  void MessageLayer::SetLocateY(int y) {
    pimpl_->SetLocateY(y);
  }

  void MessageLayer::BeginIndent() {
    pimpl_->BeginIndent();
  }

  void MessageLayer::EndIndent() {
    pimpl_->EndIndent();
  }

  void MessageLayer::SetLineSize(int px) {
    pimpl_->SetLineSize(px);
  }

  void MessageLayer::ResetLineSize() {
    pimpl_->ResetLineSize();
  }

  void MessageLayer::SetLineSpacing(int px) {
    pimpl_->SetLineSpacing(px);
  }

  void MessageLayer::ResetLineSpacing() {
    pimpl_->ResetLineSpacing();
  }

  void MessageLayer::SetDefaultStyle(const message::DefaultStyle & style) {
    pimpl_->SetDefaultStyle(style);
  }

  void MessageLayer::ResetStyle() {
    pimpl_->ResetStyle();
  }

  MessageLayer::operator std::shared_ptr<Layer>() const { return pimpl_; }

}
