﻿# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MmdNovel.h>
#include <MmdNovel/image/image_manager.h>
#include <MmdNovel/message/message_manager.h>

#include <MmdNovel/mmd/mmd_manager.h>
#include <MmdNovel/live2d/live2d_manager.h>
void Live2DUpdate();
void Main()
{
  TextureAsset::Register(L"siv3d-kun", L"Data/siv3d-kun.png");
  auto executor = std::make_shared<kag::file::FileExecutor>(L"test.txt");
  executor->AddManager<kag::file::ImageManager>(L"image");
  executor->AddManager<kag::file::MessageManager>(L"message");

  executor->AddManager<kag::file::MMDManager>(L"mmd");
  executor->AddManager<kag::live2d::Live2DManager>(L"live2d");

  TextReader reader(L"test.txt");
  Window::SetFullscreen(false, Size(1024, 768));
  Array<String> file_line;
  for ( ;;)
  {
    auto str = reader.readLine();
    if ( str )
    {
      file_line.push_back(std::move(*str));
    }
    else
    {
      break;
    }
  }
  Camera camera;
  camera.pos = { 0,12,-22 };
  camera.lookat = { 0,12,0 };
  Graphics3D::SetCamera(camera);

  Font log{ FontProperty(L"meiryo",16) };
  Graphics::SetBackground(Palette::Skyblue);
  bool isDebug = true;
  while ( System::Update() )
  {
    Live2DUpdate();
    Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
    executor->Update();
    /*if ( Input::MouseR.clicked )
      executor.CommandText(L"右クリックテスト");
*/
    executor->Draw();
    if ( Input::KeyD.clicked )
    {
      isDebug = !isDebug;
    }
    if ( isDebug )
    {
      int y = Window::Size().y;
      int line = executor->NowLine();
      while ( y > 0 && line >= 0 )
      {
        y -= log.height;
        log(file_line[line]).draw(0, y);
        log(y, y);
        line--;
      }
    }
  }


}
