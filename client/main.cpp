#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <httplib.h>
#include <iostream>

int main(int argc, const char* argv[]) {
  // 创建一个包含居中文本和一条分隔线的元素
  ftxui::Element document =
      ftxui::vbox({
          ftxui::text("Hello, ftxui!") | ftxui::bold | ftxui::center,
          ftxui::separator(),
          ftxui::text("这是一个简单的终端界面示例。") | ftxui::center,
      });

  // 创建一个屏幕对象
  ftxui::Screen screen = ftxui::Screen::Create(ftxui::Dimension::Full());

  // 将元素渲染到屏幕上
  ftxui::Render(screen, document);

  // 打印屏幕内容到终端
  screen.Print();

  return 0;
}