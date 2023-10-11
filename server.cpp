#include <iostream>
#include <string>
#include <asio.hpp>
#include <thread>

void callback(const std::error_code& error, std::size_t bytes_transferred, char recv_str[]) {

    if (error) {
        std::cout << error.message() << '\n';
    } else {
        std::cout << std::string(recv_str, bytes_transferred) << '\n';
    }
}

void send_message()
{
    std::string str{""};
    std::cout << "your message: ";
    std::cin >> str;
    std::cout << std::endl;
}

void tcp_server() 
{
    try {
        for (;;) {
            //server
            asio::io_context io_context;
            asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3303));

            asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);

            char recv_str[1024] = {};

            socket.async_receive(
                asio::buffer(recv_str),
                std::bind(callback, std::placeholders::_1, std::placeholders::_2, recv_str));
            if (recv_str != nullptr){
                send_message();
            }
            
            io_context.run();



        }
    } catch (std::exception& e) {
        std::cout << "error" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}



int main() {

    tcp_server();
    //send_message();
    return 0;
}