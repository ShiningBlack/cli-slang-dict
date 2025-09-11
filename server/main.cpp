#include <iostream>
#include <string>
#include <httplib.h>

int main() 
{
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request &req, httplib::Response &resp) {
        std::string body = "<html><body><h1>Home</h1></body></html>";
        resp.set_content(body, "text/html");
    });

    svr.Get("/hello", [](const httplib::Request &req, httplib::Response &resp) {
        std::cout << "method: " << req.method << '\t' << "path: " << req.path << std::endl;
        std::string body = "<html><body><h1>Hello World</h1></body></html>";
        resp.set_content(body, "text/html");
    });

    std::cout << "Server listening on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080); // 使用 "0.0.0.0" 更通用
    return 0; 
}