#include <websocketpp/server.hpp>
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>

using websocketpp::connection_hdl;
using websocketsvr_t = websocketpp::server<websocketpp::config::asio>;

void onOpen(connection_hdl hdl)
{
    std::cout << "websocket 握手" << std::endl;
}

void onClose(connection_hdl hdl)
{
    std::cout << "websocket 挥手" << std::endl;
}

void onMessage(websocketsvr_t* server, connection_hdl hdl, websocketsvr_t::message_ptr msg)
{
    // websocket 协议, 通过特殊的http内容切换成的，所以，server->conn中仍然保留了http的信息
    auto conn = server->get_con_from_hdl(hdl);
    const auto& request = conn->get_request();
    std::cout << request.get_method() << std::endl;
    std::cout << request.get_uri() << std::endl;
    for (auto& [k, v] : request.get_headers())
    {
        std::cout << k << " = " << v << std::endl;
    }
    std::cout << "websocket正文: " << msg->get_payload() << std::endl;
    conn->send(msg->get_payload(), msg->get_opcode());
}

void onHttp(websocketsvr_t* server, connection_hdl hdl)
{
    auto conn = server->get_con_from_hdl(hdl);
    const auto& request = conn->get_request();
    std::cout << request.get_method() << std::endl;
    std::cout << request.get_uri() << std::endl;
    std::cout << request.get_body() << std::endl;
    auto headers = request.get_headers();
    for (auto& [k, v] : headers)
    {
        std::cout << k << " = " << v << std::endl;
    }
    // 以下用来构造conn中包含的响应信息
    conn->set_status(websocketpp::http::status_code::value::ok);        // 状态码
    conn->set_body("<html><body><h1>Hello World</h1></body></html>");   // 正文
    conn->append_header("Content-Type", "text/html");                   // 响应头
}

int main(int argc, char* argv[])
{
    // 实例化服务器对象
    websocketsvr_t server{};
    // 禁用日志
    server.clear_access_channels(websocketpp::log::alevel::all);
    server.set_open_handler(onOpen);
    server.set_close_handler(onClose);
    server.set_message_handler([&server](connection_hdl hdl, websocketsvr_t::message_ptr msg){
        onMessage(&server, hdl, msg);
    });
    server.set_http_handler([&server](connection_hdl hdl){
        onHttp(&server, hdl);
    });

    server.init_asio();
    server.set_reuse_addr(true);
    server.listen(9000);
    server.start_accept();
    server.run();
    return 0;
}