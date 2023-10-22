#include <asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <algorithm>
#include <future>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>
#include <thread> 

void callback_client(
        const std::error_code& error,
        std::size_t bytes_transferred,
        std::shared_ptr<asio::ip::tcp::socket> socket,
        std::string str)
{
    if (error)
    {
        std::cout << error.message() << '\n';
    }
    else if (bytes_transferred == str.length())
    {
        std::cout << "Message is sent successfully!" << '\n';
    }
    else
    {
        socket->async_send(
                asio::buffer(str.c_str() + bytes_transferred, str.length() - bytes_transferred),
                std::bind(callback_client, std::placeholders::_1, std::placeholders::_2, socket, str));
    }
}

void callback_server(const std::error_code& error, std::size_t bytes_transferred, char recv_str[]) {

    if (error) {
        std::cout << error.message() << '\n';
    } else {
        std::cout << std::string(recv_str, bytes_transferred) << '\n';
        
    }

}

void tcp_client() 
{
    try
    {
        for (;;) {
        asio::io_context io_context;

        asio::ip::tcp::endpoint endpoint{
                asio::ip::make_address("127.0.0.1"),
                3303};

        std::shared_ptr<asio::ip::tcp::socket> socket{new asio::ip::tcp::socket{io_context}};
        
        socket->connect(endpoint);
        
        std::cout << "Connect to " << endpoint << " successfully!\n";

        std::string str{""};
        std::cout << "your message: ";
        std::cin >> str;
        std::cout << std::endl;
        if (!str.empty()) {
            socket->async_send(
                asio::buffer(str),
                std::bind(callback_client, std::placeholders::_1, std::placeholders::_2, socket, str));

        }
        
        io_context.run();
        }
    }

    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
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
                std::bind(callback_server, std::placeholders::_1, std::placeholders::_2, recv_str));


            io_context.run();



        }
    } catch (std::exception& e) {
        std::cout << "error" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

int main() {
    std::thread server_thread([](){
        tcp_server();
    });

    std::thread client_thread([](){
        tcp_client();
    });

    server_thread.join();
    client_thread.join();

    return 0;
}