# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MmdNovel.h>
void Main() {
  kag::FileExecutor executor(L"test.txt");
  auto img = executor.imageManager().GetLayer(0, kag::LayerPage::Fore);
  kag::ScaleEffect::Data data;
  data.easing.push_back(EasingController<double>(
    1., 0.5,
    Easing::Linear,	// イージング関数
    250            // 時間
    ));
  data.easing.push_back(EasingController<double>(
    0.5, 1.0,
    Easing::Linear,	// イージング関数
    250            // 時間
    ));
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