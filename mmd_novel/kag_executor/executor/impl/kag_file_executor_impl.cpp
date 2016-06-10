#include "kag_file_executor_impl.h"
namespace kag {

  FileExecutor::Pimpl::Pimpl(const Executor& exe) :executor_(exe) {
    /* メッセージ関連 */

    //tag_func_[SnapShotSpan(L"cancelautomode")] = &FileExecutor::CancelAutoModeTag;
    //tag_func_[SnapShotSpan(L"cancelskip")] = &FileExecutor::CancelSkipTag;
    tag_func_[SnapShotSpan(L"ch")] = &Pimpl::CHTag;
    tag_func_[SnapShotSpan(L"cm")] = &Pimpl::CMTag;
    tag_func_[SnapShotSpan(L"ct")] = &Pimpl::CTTag;
    tag_func_[SnapShotSpan(L"current")] = &Pimpl::CurrentTag;
    tag_func_[SnapShotSpan(L"deffont")] = &Pimpl::DefFontTag;
    tag_func_[SnapShotSpan(L"defstyle")] = &Pimpl::DefStyleTag;
    tag_func_[SnapShotSpan(L"delay")] = &Pimpl::DelayTag;
    tag_func_[SnapShotSpan(L"endindent")] = &Pimpl::EndIndentTag;
    tag_func_[SnapShotSpan(L"endnowait")] = &Pimpl::EndNoWaitTag;
    tag_func_[SnapShotSpan(L"er")] = &Pimpl::ERTag;
    tag_func_[SnapShotSpan(L"font")] = &Pimpl::FontTTag;

    //tag_func_[SnapShotSpan(L"glyph")] = &Pimpl::GlyphTag;
    //tag_func_[SnapShotSpan(L"graph")] = &Pimpl::GraphTag;
    //tag_func_[SnapShotSpan(L"hch")] = &Pimpl::HCH;
    tag_func_[SnapShotSpan(L"indent")] = &Pimpl::IndentTag;
    tag_func_[SnapShotSpan(L"l")] = &Pimpl::LTag;
    tag_func_[SnapShotSpan(L"locate")] = &Pimpl::LocateTag;

    //tag_func_[SnapShotSpan(L"locklink")] = &Pimpl::LockLinkTag;
    tag_func_[SnapShotSpan(L"nowait")] = &Pimpl::NoWaitTag;
    tag_func_[SnapShotSpan(L"p")] = &Pimpl::PTag;
    tag_func_[SnapShotSpan(L"position")] = &Pimpl::PositionTTag;
    tag_func_[SnapShotSpan(L"r")] = &Pimpl::RTag;
    tag_func_[SnapShotSpan(L"resetfont")] = &Pimpl::ResetFontTag;
    tag_func_[SnapShotSpan(L"resetstyle")] = &Pimpl::ResetStyleTag;

    //tag_func_[SnapShotSpan(L"ruby")] = &Pimpl::RubyTag;
    tag_func_[SnapShotSpan(L"style")] = &Pimpl::StyleTag;

    //tag_func_[SnapShotSpan(L"unlocklink")] = &Pimpl::UnlockLinkTag;

    /* 画像関連 */
    tag_func_[SnapShotSpan(L"image")] = &Pimpl::ImageTag;

    /* レイヤ関連 */
    tag_func_[L"move"] = &Pimpl::MoveTag;
    tag_func_[L"trans"] = &Pimpl::TransTag;

    /* MMD関連 */
    tag_func_[L"mmd"] = &Pimpl::MMDTag;
    tag_func_[L"camera"] = &Pimpl::CameraTag;

  }

  void FileExecutor::Pimpl::Load(const FilePath & path) {
    parser_ = Parser(path);
  }

  void FileExecutor::Pimpl::Update() {
    if (!executor_.Update()) return;

    while (executor_.CommandUpdate()) {
      switch (parser_.nextType()) {
      case  kag::Parser::Type::Text:
        executor_.CommandText(parser_.readText());
        break;

      case  kag::Parser::Type::Command:
      {
        auto token = parser_.readCommand();
        auto func = tag_func_[token.name()];
        if (func) {
          (this->*func)(token);
        } else {
          throw std::runtime_error(token.name().ToNarrow());
        }
        break;
      }

      case  kag::Parser::Type::EndOfStream:
        return;
      }
    }
  }

  /* - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  -
  タグの実装
  - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - - - - -  - - -  - */
  void FileExecutor::Pimpl::LTag(Parser::CommandToken &) {
    executor_.CommandL();
  }

  void FileExecutor::Pimpl::LocateTag(Parser::CommandToken & token) {
    Value<int> x, y;
    auto& args = token.arguments();
    using namespace converter;
    args.ValTo(L"x", ToInt10, [&](int val) { x = val; });
    args.ValTo(L"y", ToInt10, [&](int val) { y = val; });

    args.IfNotEmptyException();
    executor_.CommandLocate(x, y);
  }

  void FileExecutor::Pimpl::RTag(Parser::CommandToken &) {
    executor_.CommandR();
  }

  void FileExecutor::Pimpl::ResetFontTag(Parser::CommandToken &) {
    executor_.CommandResetFont();
  }

  void FileExecutor::Pimpl::ResetStyleTag(Parser::CommandToken &) {
    executor_.CommandResetStyle();
  }

  void FileExecutor::Pimpl::StyleTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandStyle([=](StyleCommandEditor& editor) mutable {
      args.Val(L"linesize", [&](const SnapShotSpan& val) {
        if (val == L"default") {
          editor.linesize();
        } else {
          editor.linesize(converter::ToInt10(val));
        }
      });

      args.ValTo(L"linespacing", converter::ToInt10, [&](int val) {
        editor.linespacing(val);
      });

      args.IfNotEmptyException();
    });
  }

  /* レイヤ関連 */

  void FileExecutor::Pimpl::ImageTag(Parser::CommandToken & token) {
    auto& args = token.arguments();

    //executor_.CommandImage(layer.second, page, tex);
    executor_.Command([args = std::move(args), executor = executor_]() mutable{
      using namespace converter;
      auto layer = ToLayerNum(args.find_or_throw(L"layer"));
      assert(layer.first == LayerType::Foreground);

      LayerPage page = args.ValOrDefaultTo(L"page", ToPage, LayerPage::Fore);
      auto img_layer = executor.imageManager()->GetLayer(layer.second, page);
      img_layer->SetTex(Texture(args.find_or_throw(L"storage").ToStr()));

      args.ValTo(L"left", ToInt10, img_layer.bind(&Layer::SetPositionLeft));
      args.ValTo(L"top", ToInt10, img_layer.bind(&Layer::SetPositionTop));
      args.ValTo(L"opacity", ToInt10, img_layer.bind(&Layer::SetOpacity));
      args.ValTo(L"visible", ToBool, img_layer.bind(&Layer::IsVisible));
      args.ValTo(L"index", ToInt10, [&img_layer](int z_index) {
        uint8 z = static_cast<uint8>(Max(0, Min(z_index, 255)));
        img_layer->SetZIndex(z);
      });
    });
  }

  void FileExecutor::Pimpl::MoveTag(Parser::CommandToken & token) {
    executor_.Command([executor = executor_, args = std::move(token.arguments())]() mutable {
      using namespace converter;
      auto layer_num = ToLayerNum(args.find_or_throw(L"layer"));
      auto page = args.ValOrDefaultTo(L"page", ToPage, LayerPage::Fore);
      int time = ToInt10(args.find_or_throw(L"time"));
      LayerPtr layer = executor.GetLayer(layer_num, page);

      auto path = args.find_or_throw(L"path");
      auto buf = path.Str();
      auto len = path.Length();
      int n = static_cast<int>(std::count(buf, buf + len, L'('));
      Vec2 s, e;
      s = layer->position().pos;
      int n_pos = 0;
      MoveEffectData::Array data;

      for (int i = 0; i < n; i++) {
        int opacity;
        if (swscanf(buf + n_pos, L"(%lf,%lf,%d)%n", &e.x, &e.y, &opacity, &n_pos) != 3) {
          throw std::runtime_error(path.ToNarrow());
        }
        data.push_back(MoveEffectData(s, e, Easing::Linear, EasingType::Type::In, time));
        std::swap(s, e);
      }
      layer->MoveEffect(data);
    });
  }

  void FileExecutor::Pimpl::TransTag(Parser::CommandToken & token) {
    executor_.Command([executor = executor_, args = std::move(token.arguments())]() mutable {
      using namespace converter;
      const auto layer_num = args.ValOrDefaultTo(L"layer", ToLayerNum, std::make_pair(LayerType::Background, 0));
      const int time_millisec = ToInt10(args.find_or_throw(L"time"));
      SnapShotSpan method = args.ValOrDefault(L"method", SnapShotSpan(L"universal"));
      auto layer = executor.GetLayer(layer_num);
      if (method == L"universal") {
        TransUniversalData data;
        data.rule_tex = Texture(args.find_or_throw(L"rule").ToStr());
        data.vague = ToInt10(args.find_or_throw(L"vague"));
        data.time_millisec = time_millisec;
        layer.Trans(data);
      } else if (method == L"crossfade") {
        layer.Trans(time_millisec);
      } else if (method == L"scroll") {

        // TODO:
        throw std::runtime_error(method.ToNarrow());
      } else {
        throw std::runtime_error(method.ToNarrow());
      }

    });

  }

  /* MMD関連 */

  void FileExecutor::Pimpl::MMDTag(Parser::CommandToken & token) {
    executor_.Command([executor = executor_, args = std::move(token.arguments())]() mutable {
      using namespace converter;
      auto page = args.ValOrDefaultTo(L"page", ToPage, LayerPage::Fore);
      auto layer = executor.mmdLayer()[page];
      auto file = args.find_or_throw(L"storage");
      auto visible = args.ValOrDefaultTo(L"visible", ToBool, true);
      layer->IsVisible(visible);
      layer->SetModel(file.ToStr());
      args.Val(L"vmd", [&layer](const SnapShotSpan& val) {
        layer->SetVMD(s3d_mmd::VMD(val.ToStr()));
      });
    });
  }

  void FileExecutor::Pimpl::CameraTag(Parser::CommandToken & token) {
    executor_.Command([executor = executor_, args = std::move(token.arguments())]() mutable {
      Camera camera;
      args.Val(L"pos", [&camera](const SnapShotSpan& val) {
        Vec3 pos;
        if (swscanf(val.Str(), L"(%lf,%lf,%lf)", &pos.x, &pos.y, &pos.z) == 3) {
          camera.pos = pos;
        }
      });
      args.Val(L"lookat", [&camera](const SnapShotSpan& val) {
        Vec3 lookat;
        if (swscanf(val.Str(), L"(%lf,%lf,%lf)", &lookat.x, &lookat.y, &lookat.z) == 3) {
          camera.lookat = lookat;
        }
      });
      Graphics3D::SetCamera(camera);
    });
  }

  void FileExecutor::Pimpl::NoWaitTag(Parser::CommandToken &) {
    executor_.CommandNoWait();
  }

  void FileExecutor::Pimpl::PTag(Parser::CommandToken &) {
    executor_.CommandP();
  }

  void FileExecutor::Pimpl::DelayTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    args.Val(L"speed", [&](const SnapShotSpan& val) {
      if (val == L"user") {
        executor_.CommandDelay(30);
      } else if (val == L"nowait") {
        executor_.CommandNoWait();
      } else {
        executor_.CommandDelay(converter::ToInt10(val));
      }
    }, []() {
      throw std::runtime_error("[delay] にはspeed属性が必須");
    });

    args.IfNotEmptyException();
  }

  void FileExecutor::Pimpl::EndIndentTag(Parser::CommandToken &) {
    executor_.CommandEndIndent();
  }

  void FileExecutor::Pimpl::EndNoWaitTag(Parser::CommandToken &) {
    executor_.CommandEndNoWait();
  }

  void FileExecutor::Pimpl::ERTag(Parser::CommandToken &) {
    executor_.CommandER();
  }

  void FileExecutor::Pimpl::CMTag(Parser::CommandToken &) {
    executor_.CommandCM();
  }

  void FileExecutor::Pimpl::CTTag(Parser::CommandToken &) {
    executor_.CommandCT();
  }

  void FileExecutor::Pimpl::CurrentTag(Parser::CommandToken & token) {
    Value<int> layer;
    LayerPage page = LayerPage::Fore;

    auto& args = token.arguments();
    using namespace converter;
    args.ValTo(L"layer", ToMessageLayerNum, [&](int val) { layer = val; });
    args.ValTo(L"page", ToPage, [&](LayerPage val) { page = val; });
    args.IfNotEmptyException();
    executor_.CommandCurrent(layer(), page);
  }

  void FileExecutor::Pimpl::DefFontTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefFont([args](FontCommandEditor& editor) mutable {
      using namespace converter;
      args.Val(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.ValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.ValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.ValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

      args.ValTo(L"shadow", ToBool, [&](bool val) { editor.is_shadow(val); });

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::DefStyleTag(Parser::CommandToken & token) {
    auto& args = token.arguments();
    executor_.CommandDefStyle([args = std::move(args)](DefaultStyleCommandEditor& editor)mutable {
      args.Val(L"linesize", [&](const SnapShotSpan& val) {
        if (val == L"default") {
          editor.linesize();
        } else {
          editor.linesize(converter::ToInt10(val));
        }
      });

      args.ValTo(L"linespacing", converter::ToInt10, [&](int val) {
        editor.linespacing(val);
      });

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::FontTTag(Parser::CommandToken & token) {

    auto& args = token.arguments();
    executor_.CommandFont([args = std::move(args)](FontCommandEditor& editor)mutable {
      using namespace converter;
      args.Val(L"face", [&](const SnapShotSpan& val) { editor.face(val.ToStr()); });

      args.ValTo(L"size", ToInt10, [&](int val) { editor.size(val); });

      args.ValTo(L"italic", ToBool, [&](bool val) { editor.is_italic(val); });

      args.ValTo(L"bold", ToBool, [&](bool val) { editor.is_bold(val); });

      args.ValTo(L"color", ToColor, [&](const Color& val) {editor.color(val); });

      args.ValTo(L"shadow", ToBool, [&](bool val) {editor.is_shadow(val); });

      args.ValTo(L"shadowcolor", ToColor, [&](const Color& val) { editor.shadowcolor(val); });

      args.IfNotEmptyException();
    });

  }

  void FileExecutor::Pimpl::IndentTag(Parser::CommandToken &) {
    executor_.CommandIndent();
  }

  void FileExecutor::Pimpl::PositionTTag(Parser::CommandToken & token) {
    using namespace converter;
    Value<int> index;
    Value<LayerPage> page;
    auto& args = token.arguments();
    args.ValTo(L"layer", ToMessageLayerNum, [&](int val) { index = val; });

    args.ValTo(L"page", ToPage, [&](LayerPage val) { page = val; });

    executor_.CommandPosition(index, page, [args = std::move(args)](PositionCommandEditor& editor) mutable {
      args.ValTo(L"left", ToInt10, [&](int val) { editor.position_left(val); });

      args.ValTo(L"top", ToInt10, [&](int val) { editor.position_top(val); });

      args.ValTo(L"width", ToInt10, [&](int val) { editor.position_width(val); });

      args.ValTo(L"height", ToInt10, [&](int val) { editor.position_height(val); });

      args.ValTo(L"marginl", ToInt10, [&](int val) { editor.margin_left(val); });

      args.ValTo(L"margint", ToInt10, [&](int val) { editor.margin_top(val); });

      args.ValTo(L"marginr", ToInt10, [&](int val) { editor.margin_right(val); });

      args.ValTo(L"marginb", ToInt10, [&](int val) { editor.margin_bottom(val); });

      args.ValTo(L"color", ToColor, [&](const Color& col) {
        editor.color(col.r, col.g, col.b);
      });

      args.Val(L"frame", [&](const SnapShotSpan& val) {
        if (val.Length() != 0) {
          editor.frame(Texture(val.ToStr()));
        } else {
          editor.frame(Texture());
        }
      });

      args.ValTo(L"opacity", ToInt10, [&](int val) { editor.opacity(val); });

      args.ValTo(L"visible", ToBool, [&](bool val) { editor.visible(val); });

      args.IfNotEmptyException();
    });
  }

  void FileExecutor::Pimpl::CHTag(Parser::CommandToken & token) {
    executor_.CommandTextNoDelay(token.arguments().find_or_throw(L"text"));
  }

}
