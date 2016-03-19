#pragma once
#include <kag_executor/message_manager.h>
namespace kag {
  struct Default {
    constexpr operator int() const { return -1; }
  };
  constexpr Default default;

  template<class Type> class Value {
  public:
    constexpr Value(const Type& v) :t(v) {}
    constexpr Value() : t(kag::default) {}
    constexpr operator const Type&() const { return t; }
    Type& operator=(const Type& v) {
      t = v;
      return t;
    }
    constexpr bool operator==(const Default& def) const {
      return t == def;
    }
  private:

    Type t;
  };
  template<class Type> constexpr bool operator==(const Type& t, const Default&) {
    return t == static_cast<Type>(kag::default);
  }
  class Executor;
  class FontCommandEditor {
    friend Executor;
    using This = FontCommandEditor;
    FontCommandEditor(MessageManager& manager);

    void StyleCheck(FontStyle style);
    FontStyle CreateStyle();
  public:
    This& size(int size) { size_ = size; return *this; }
    This& face(const String name) { face_ = name; return *this; }
    This& color(const Color& color) { color_ = color; return *this; }
    This& is_italic(bool is_italic) { is_italic_ = is_italic; return *this; }
    This& is_bold(bool is_bold) { is_bold_ = is_bold; return *this; }

    void Commit();
  private:
    int size_;
    String face_;
    Color color_;
    bool is_italic_;
    bool is_bold_;

    //未実装
    int ruby_size_;
    int ruby_offset_;
    bool is_shadow_;
    Color shadow_color_;
    bool is_edge_;
    Color edge_color_;

    MessageManager& manager_;
  };

  class PositionCommandEditor {
    friend Executor;
    using This = PositionCommandEditor;
    PositionCommandEditor(MessageLayer& layer) :layer_(layer) {
    }

  public:
    This& position_left(int val) { layer_.SetPositionLeft(val); return *this; }
    This& position_top(int val) { layer_.SetPositionTop(val); return *this; }
    This& position_width(int val) { layer_.SetPositionWidth(val); return *this; }
    This& position_height(int val) { layer_.SetPositionHeight(val); return *this; }

    This& margin_left(int val) { layer_.SetMarginLeft(val); return *this; }
    This& margin_top(int val) { layer_.SetMarginTop(val); return *this; }
    This& margin_right(int val) { layer_.SetMarginRight(val); return *this; }
    This& margin_bottom(int val) { layer_.SetMarginBottom(val); return *this; }

    void Commit() {
      layer_.Clear();
    }

  private:
    MessageLayer& layer_;
  };
}
