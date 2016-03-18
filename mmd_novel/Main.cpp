# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <kag_executor/kag_executor.h>
void Main() {
  kag::Executor executor(L"test.txt");

  while (System::Update()) {
    Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
    executor.Update();
    executor.Draw();
  }
}
