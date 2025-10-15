// main.cpp
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace ftxui;

// --- 简单 URL encode helper（不用依赖 cpp-httplib 的 internal 符号） ---
static std::string url_encode(const std::string& s) {
    std::ostringstream out;
    out.fill('0');
    out << std::hex << std::uppercase;
    for (unsigned char c : s) {
        // unreserved characters according to RFC3986
        if ( (c >= '0' && c <= '9') ||
             (c >= 'A' && c <= 'Z') ||
             (c >= 'a' && c <= 'z') ||
             c == '-' || c == '_' || c == '.' || c == '~' ) {
            out << static_cast<char>(c);
        } else {
            out << '%' << std::setw(2) << int(c);
        }
    }
    // restore defaults (not strictly necessary here)
    return out.str();
}

// --- 解析结构（示例） ---
// 如果你已经在 protocol.hpp 中定义了 slang_dict::SlangDefinition / from_json，优先使用你的实现。
// 这里提供一个通用的 fallback 解析，方便单文件测试。
struct SlangDefinition {
    std::string term;
    std::string definition;
    std::string origin;
};
static bool parse_slang_from_json(const std::string& body, SlangDefinition& out) {
    try {
        auto j = nlohmann::json::parse(body);
        // 根据你的服务器响应结构调整字段名
        out.term = j.value("term", "");
        out.definition = j.value("definition", "");
        out.origin = j.value("origin", "");
        return true;
    } catch (...) {
        return false;
    }
}

// --- 全局 UI 状态（为了示例简单使用全局变量） ---
static std::string input_text;
static std::string query_result = "在此显示查询结果...";
static std::string status_message = "输入流行语并按回车查询...";

// 执行查询（网络 IO），**注意**：这个函数是线程安全地被后台线程调用，
// 它会修改全局字符串（示例简单处理），调用方（主线程）需在收到 Event::Custom 后重绘。
static void perform_query_blocking(const std::string& term) {
    if (term.empty()) {
        status_message = "查询词不能为空！";
        query_result.clear();
        return;
    }

    status_message = "正在查询: " + term + "...";

    // 使用 cpp-httplib 进行同步 GET
    // 注意：host/port 根据你的服务实际调整
    httplib::Client cli("localhost", 8080);

    std::string path = "/api/slang?term=" + url_encode(term);

    auto res = cli.Get(path.c_str());
    if (res && res->status == 200) {
        SlangDefinition def;
        if (parse_slang_from_json(res->body, def)) {
            std::ostringstream ss;
            ss << "词语: " << def.term << "\n"
               << "定义: " << def.definition << "\n"
               << "来源: " << def.origin;
            query_result = ss.str();
            status_message = "查询成功！";
        } else {
            query_result.clear();
            status_message = "服务器响应解析失败（非 JSON 或 字段缺失）。";
        }
    } else {
        query_result.clear();
        if (res) {
            if (res->status == 404) {
                status_message = "未找到流行语: " + term;
            } else {
                status_message = "查询失败，状态码: " + std::to_string(res->status);
            }
        } else {
            status_message = "网络请求失败，请检查服务器是否运行。";
        }
    }
}

int main1() {
    auto screen = ScreenInteractive::Fullscreen();

    // Input 选项：按回车时把网络请求放到后台线程执行，返回后发 Event::Custom 通知刷新
    InputOption input_option;
    input_option.on_enter = [&] {
        // 拷贝输入词到局部副本，避免捕获引用在后台线程中出现竞态
        const std::string term = input_text;
        // 先把状态设置为正在查询（视觉反馈），并要求重绘
        status_message = "已提交查询任务...";
        screen.PostEvent(Event::Custom);

        // 后台线程执行阻塞网络请求
        std::thread([term, &screen]() {
            perform_query_blocking(term);
            // 查询完成后通知主线程重绘（使用 Event::Custom）
            screen.PostEvent(Event::Custom);
        }).detach();
    };

    auto input_component = Input(&input_text, "输入流行语...", input_option);

    // result / status components：使用单参数 (bool) lambda 的 Renderer 重载
    auto result_component = Renderer([&](bool) {
        // 使用 paragraph 分行显示或直接 text + vbox
        return vbox({
            text(query_result) | flex  // 结果区域可伸缩
        });
    });

    auto status_component = Renderer([&](bool) {
        // 使用 FTXUI 中存在的颜色常量（注意是 Grey 而非 Gray）
        return text(status_message) | color(Color::Grey50);
    });

    // 主容器
    auto container = Container::Vertical({
        input_component,
        result_component,
        status_component,
    });

    // 主渲染器（传入 container 作为 child，render lambda 不需要 bool 参数）
    auto renderer = Renderer(container, [&] {
        return vbox({
            text("流行语查询 CLI") | bold | hcenter | size(HEIGHT, EQUAL, 2),
            separator(),
            hbox(text("查询词: "), input_component->Render()) | border,
            separator(),
            text("查询结果:") | bold,
            result_component->Render() | border | flex,
            separator(),
            status_component->Render(),
            text("按 Esc 键退出") | hcenter | color(Color::Grey37)
        }) | border | flex;
    });

    // 捕获 Esc 键用于退出：调用 screen.ExitLoopClosure()() 来退出
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            // ExitLoopClosure 返回一个可调用对象，调用它会退出主循环
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    // 运行主循环
    screen.Loop(renderer);
    return 0;
}


// 假设这是获取串口列表的函数
std::vector<std::string> get_available_serial_ports() {
    // 实际实现会调用底层库
    return {"/dev/ttyS0", "/dev/ttyUSB0", "/dev/ttyUSB1", "COM3"};
}

int main() {
    using namespace ftxui;

    // 获取串口列表
    std::vector<std::string> ports = get_available_serial_ports();
    int selected = 0;
    std::string chosen_port;

    auto screen = ScreenInteractive::TerminalOutput();

    // Radiobox 控件：单选列表
    auto radiobox = Radiobox(&ports, &selected);

    // 渲染界面
    auto renderer = Renderer(radiobox, [&] {
        return vbox({
            text("请选择串口:") | bold,
            separator(),
            radiobox->Render(),
            separator(),
            text("按 Enter 确认选择，按 q 退出"),
        }) | border | center;
    });

    // 捕获用户事件
    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Return) {
            chosen_port = ports[selected];
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    // 进入事件循环
    screen.Loop(component);

    // 输出结果
    if (!chosen_port.empty()) {
        std::cout << "你选择的串口是: " << chosen_port << std::endl;
    } else {
        std::cout << "未选择串口。" << std::endl;
    }

    return 0;
}
