#pragma once
#include <MmdNovel/message/message_manager.h>
namespace kag {

  class Executor;
  class FontCommandEditor {
    friend Executor;
    using This = FontCommandEditor;

    void StyleCheck(FontStyle style);
    FontStyle CreateStyle();
    virtual void commit(const message::TextFont& font) {
      manager_.Current()->SetFont(font);

    }

    void Commit();
  protected:
    FontCommandEditor(MessageManager& manager);

  public:

    This& size(int size) { size_ = size; return *this; }
    This& face(const String name) { face_ = name; return *this; }
    This& color(const Color& color) { color_ = color; return *this; }
    This& is_italic(bool is_italic) { is_italic_ = is_italic; return *this; }
    This& is_bold(bool is_bold) { is_bold_ = is_bold; return *this; }
    This& is_shadow(bool is_shadow) { is_shadow_ = is_shadow; return *this; }
    This& shadowcolor(const Color& shadow_color) { shadow_color_ = shadow_color; return *this; }

  protected:

    int size_;
    String face_;
    Color color_;
    bool is_italic_;
    bool is_bold_;

    bool is_shadow_;
    Color shadow_color_;

    //未実装
    int ruby_size_;
    int ruby_offset_;
    bool is_edge_;
    Color edge_color_;

    MessageManager& manager_;
  };

  class DefFontCommandEditor : public FontCommandEditor {
    friend Executor;
    void commit(const message::TextFont& font) override {
      manager_.Current()->SetDefaultFont(font);
    }
    using FontCommandEditor::FontCommandEditor;
  };

  class PositionCommandEditor {
    friend Executor;
    using This = PositionCommandEditor;
    PositionCommandEditor(MessageLayer& layer) :layer_(layer) {
    }
    void Commit() {
      layer_->Clear();
    }
  public:
    This& position_left(int val) { layer_->SetPositionLeft(val); return *this; }
    This& position_top(int val) { layer_->SetPositionTop(val); return *this; }
    This& position_width(int val) { layer_->SetPositionWidth(val); return *this; }
    This& position_height(int val) { layer_->SetPositionHeight(val); return *this; }

    This& margin_left(int val) { layer_->SetMarginLeft(val); return *this; }
    This& margin_top(int val) { layer_->SetMarginTop(val); return *this; }
    This& margin_right(int val) { layer_->SetMarginRight(val); return *this; }
    This& margin_bottom(int val) { layer_->SetMarginBottom(val); return *this; }

    This& frame(Texture tex) { layer_->SetBackgroundTex(tex); return*this; }
    This& color(int r, int g, int b) { layer_->SetBackgroundRGB(r, g, b); return*this; }
    This& opacity(int a) { layer_->SetBackgroundOpacity(a); return*this; }
    This& visible(bool visible) { layer_->IsVisible(visible); return *this; }
  private:
    //TODO: 未実装
    This& draggable() {}
    This& vertical() {}
    This& framekey() {}
  private:
    MessageLayer& layer_;
  };

  class StyleCommandEditor {
    friend Executor;
    using This = StyleCommandEditor;
  protected:
    StyleCommandEditor(MessageManager& manager) : layer_(manager.Current()) {
    }
    void Commit() {}
  public:

    This& linespacing(int px) { layer_->SetLineSpacing(px); return *this; }
    This& linesize(int px) { layer_->SetLineSize(px); return *this; }
    This& linesize() { layer_->ResetLineSize(); return *this; }
  private:

    // 未実装
    //This& autoreturn(bool is_auto_return) {}
    //This& align(AlignType type) {}
    //This& pitch(int px) {}

    MessageLayer& layer_;
  };
  class DefaultStyleCommandEditor {
    friend Executor;
    using This = DefaultStyleCommandEditor;
  protected:

    DefaultStyleCommandEditor(MessageManager& manager) :layer_(manager.Current()) {}
    void Commit() {
      layer_->SetDefaultStyle(style_);
    }
  public:

    This& linespacing(int px) { style_.line_spacing_ = px; return *this; }
    This& linesize(int px) { style_.line_size_ = px; return *this; }
    This& linesize() { style_.ResetLineSize(); return *this; }

  private:

    message::DefaultStyle style_;
    MessageLayer& layer_;
  };
}
