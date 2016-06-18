#include <MmdNovel/message/message_layer.h>
namespace kag
{
  namespace message
  {
    namespace pimpl
    {
      void MessageLayerPimpl::clone(LayerPtr & out)
      {
        DefaultClone(*this, out);
      }

      MessageLayerPimpl::MessageLayerPimpl(const LayerManager & manager) :ILayer(manager)
      {
        SetPositionLeft(16);
        SetPositionTop(16);
        SetPositionWidth(Window::Size().x - 32);
        SetPositionHeight(Window::Size().y - 32);
        IsVisible(false);
        margin_.set({ 8,8 }, { 8,8 });
        background_color_ = Palette::Gray;
        background_color_.a = 128;
        indent_width_ = InvalidIndent;
        Clear();
      }

      void MessageLayerPimpl::Clear()
      {
        limit_line_num = 0;
        NextPage();
      }

      void MessageLayerPimpl::NextPage()
      {
        text_line_.clear();
        limit_line_num = 0;
        sum_height_ = 0;
        if ( overflow_text )
        {
          text_line_.emplace_back(0, *overflow_text);
          overflow_text = none;
          CheckByReturn();
        }
        else
        {
          text_line_.emplace_back(0, message::Text(now_font_));
        }
        limit_line_num = 1;
      }

      void MessageLayerPimpl::Append(const String & str)
      {
        text_line_.back().Append(str);
        CheckByReturn();
      }

      void MessageLayerPimpl::Append(const wchar & str)
      {
        text_line_.back().Append(str);

        //Print(str);
        CheckByReturn();
      }

      bool MessageLayerPimpl::IsLimitHeihgt()
      {
        return position().h - margin_.h - margin_.y <= sum_height_ + text_line_.back().Height();
      }

      void MessageLayerPimpl::AppenNewLine()
      {
        sum_height_ += text_line_.back().Height();
        text_line_.emplace_back(sum_height_, TextLine(indent_width_ == InvalidIndent ? 0 : indent_width_, message::Text(now_font_)));
        if ( !IsLimitHeihgt() )
        {
          limit_line_num = static_cast<int>(text_line_.size());
        }
      }

      void MessageLayerPimpl::draw() const
      {
        auto& pos = position();
        pos.draw(background_color_);
        if ( !background_tex_.isEmpty() )
          pos(background_tex_).draw();

        int y = position().y + margin_.y;
        for ( auto& i : step(limit_line_num) )
        {
          text_line_[i].Draw(pos.x + margin_.x, y + text_line_[i].y_);
        }
      }

      const kag::message::TextFont & MessageLayerPimpl::NowFont() const { return now_font_; }

      void MessageLayerPimpl::SetFont(const message::TextFont & font)
      {
        text_line_.back().AppendNewFont(font);
        now_font_ = font;
      }

      void MessageLayerPimpl::SetDefaultFont(const message::TextFont & font)
      {
        default_font_ = font;
      }

      void MessageLayerPimpl::ResetFont()
      {
        SetFont(default_font_);
      }

      void MessageLayerPimpl::SetMarginTop(int top)
      {
        margin_.y = top;
      }

      void MessageLayerPimpl::SetMarginLeft(int left)
      {
        margin_.x = left;
      }

      void MessageLayerPimpl::SetMarginRight(int width)
      {
        margin_.w = width;
      }

      void MessageLayerPimpl::SetMarginBottom(int height)
      {
        margin_.h = height;
      }

      void MessageLayerPimpl::SetBackgroundColor(Color argb)
      {
        background_color_ = argb;
      }

      void MessageLayerPimpl::SetBackgroundRGB(int r, int g, int b)
      {
        const int a = background_color_.a;
        background_color_ = Color(r, g, b, a);
      }

      void MessageLayerPimpl::SetBackgroundOpacity(int a)
      {
        background_color_.a = a;
      }

      void MessageLayerPimpl::SetBackgroundTex(Texture tex)
      {
        background_tex_ = tex;
      }

      void MessageLayerPimpl::SetLocate(int x, int y)
      {
        text_line_.emplace_back(y, message::TextLine(x, now_font_));
        sum_height_ = y;
        limit_line_num++;
      }

      void MessageLayerPimpl::SetLocateX(int x)
      {
        text_line_.back().AppendNewFont(x, now_font_);
        limit_line_num++;
      }

      void MessageLayerPimpl::SetLocateY(int y)
      {
        SetLocate(0, y);
      }

      void MessageLayerPimpl::EndIndent()
      {
        indent_width_ = InvalidIndent;
      }

      void MessageLayerPimpl::SetLineSize(int px)
      {
        text_line_.back().SetLineSize(px);
      }

      void MessageLayerPimpl::ResetLineSize()
      {
        text_line_.back().ResetLineSize();
      }

      void MessageLayerPimpl::SetLineSpacing(int px)
      {
        text_line_.back().SetLineSpacing(px);
      }

      void MessageLayerPimpl::ResetLineSpacing()
      {
        text_line_.back().ResetLineSpacing();
      }

      void MessageLayerPimpl::SetDefaultStyle(const message::DefaultStyle & style)
      {
        default_style_ = style;
      }

      void MessageLayerPimpl::ResetStyle()
      {
        SetLineSpacing(default_style_.line_spacing_);
        SetLineSize(default_style_.line_size_);
      }

      void MessageLayerPimpl::CheckByReturn()
      {
        assert(!IsLimitHeihgt());
        int line_spacing = text_line_.back().LineSpacing();
        for ( ;;)
        {
          auto opt = text_line_.back().ByReturn(position().w - margin_.x - margin_.w);
          if ( !opt )
            return;

          int indent = 0;
          if ( indent_width_ != InvalidIndent )
            indent = indent_width_;
          sum_height_ += text_line_.back().Height();
          if ( !IsLimitHeihgt() )
          {
            text_line_.emplace_back(sum_height_, message::TextLine(indent, *opt));
            text_line_.back().SetLineSpacing(line_spacing);
            limit_line_num = static_cast<int>(text_line_.size());
          }
          else
          {
            overflow_text = std::move(opt);
            return;
          }
        }
      }

    }
  }

}
