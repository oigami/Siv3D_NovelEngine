# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MmdNovel.h>
void Main() {
  kag::FileExecutor executor(L"test.txt");
  auto img = executor.imageManager().GetLayer(0, kag::LayerPage::Fore);
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
      img->ScaleEffect(data);
    }
    executor.Draw();
  }
}