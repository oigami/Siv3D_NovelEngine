# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MmdNovel.h>
void Main() {
  TextureAsset::Register(L"siv3d-kun", L"Data/siv3d-kun.png");
  kag::FileExecutor executor(L"test.txt");

  while (System::Update()) {
    Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
    executor.Update();
    if (Input::MouseR.clicked)
      executor.CommandText(L"右クリックテスト");

    executor.Draw();
  }


}