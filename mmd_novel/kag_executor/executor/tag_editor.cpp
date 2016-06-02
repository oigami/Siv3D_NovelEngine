#include <MmdNovel/tag_editor.h>
namespace kag {
  FontCommandEditor::FontCommandEditor(MessageManager & manager) :manager_(manager) {
    auto& now_font = manager.Current()->NowFont();
    size_ = now_font.font_.size();
    face_ = now_font.font_.name();
    color_ = now_font.color_;
    shadow_color_ = now_font.shadow_color_;
    is_shadow_ = now_font.is_shadow_;
    StyleCheck(now_font.font_.style());
  }
  void FontCommandEditor::StyleCheck(FontStyle style) {
    is_italic_ = false;
    is_bold_ = false;
    switch (style) {
    case s3d::FontStyle::BitmapBoldItalic:
    case s3d::FontStyle::BoldItalic:
      is_italic_ = true;
    case s3d::FontStyle::BitmapBold:
    case s3d::FontStyle::Bold:
      is_bold_ = true;
      break;
    case s3d::FontStyle::BitmapItalic:
    case s3d::FontStyle::Italic:
      is_italic_ = true;
    default:
      break;
    }
  }
  FontStyle FontCommandEditor::CreateStyle() {
    int style_cnt = (is_italic_ == true) + (is_italic_ == true) * 2;
    switch (style_cnt) {
    case 1: return FontStyle::Italic;
    case 2: return FontStyle::Bold;
    case 3: return FontStyle::BoldItalic;
    default:return FontStyle::Regular;
    }
  }
  void FontCommandEditor::Commit() {
    FontProperty prop;
    prop.name = std::move(face_);
    prop.size = size_;
    prop.style = CreateStyle();
    message::TextFont new_font{ Font(prop),color_ };
    new_font.is_shadow_ = is_shadow_;
    new_font.shadow_color_ = shadow_color_;
    commit(new_font);
  }



}
