﻿# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MmdNovel.h>
void Main() {
  TextureAsset::Register(L"siv3d-kun", L"Data/siv3d-kun.png");
  kag::FileExecutor executor(L"test.txt");
  auto img = executor.imageManager()->GetLayer(0, kag::LayerPage::Fore);
  kag::ScaleEffectData::Array data;
  kag::ScaleEffectData tmp;
  tmp.start = 1.0;
  tmp.end = 0.5;
  data.push_back(tmp);
  tmp.start = 0.5;
  tmp.end = 1.0;
  data.push_back(tmp);
  Rect pos(0, 0, -1000000000, 0);
  while (System::Update()) {
    Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
    executor.Update();
    pos.draw();
    if (Input::MouseR.clicked)
      executor.CommandText(L"右クリックテスト");
    if (Input::MouseM.clicked) {
      auto img_layer = executor.imageManager()->GetLayer(0);

      img_layer.Back()->SetTex(TextureAsset(L"siv3d-kun"));
      img_layer.Back()->SetPositionLeft(40);
      img_layer.Back()->IsVisible(true);
      img_layer.Back()->SetOpacity(255);
      kag::TransUniversalData data;
      data.time_millisec = 9000;
      Image img(L"Data/rule1080p/002.png");
      for (auto& i : img) {
        i.a = i.g;
      }
      Texture rule(img);
      data.rule_tex = rule;
      img_layer.Trans(data);

      //img->ScaleEffect(data);
    }
    executor.Draw();
  }
}