# include <Siv3D.hpp>
# include <HamFramework.hpp>
#include <MMD.h>
#include <kag_tokenizer/tokenizer.h>
void Main() {
  const Font font(30);
  kag::Tokenizer parser;
  TextReader reader(L"test.txt");
  Console::Open();
  parser.Initialize(reader.readAll());
  while (parser.NextToken().Type() != kag::KAGTokenType::EndOfStream) {
    using namespace std;
    wcout << L'<' << parser.Read().Span() << L'>' << endl;
  }
  using namespace s3d_mmd;

  MMD model(L"Data/初音ミク/初音ミク.pmd");

  String text = L"ようこそ、Siv3D の世界へ！";
  while (System::Update()) {
    model.draw();
    font(text).draw();
    if (Input::MouseL.clicked) {
      text = parser.Read().Span().ToStr();
    }
    Circle(Mouse::Pos(), 50).draw({ 255, 0, 0, 127 });
  }
}
