#include <asio.hpp>
#include <functional>
#include <iostream>
#include <memory>

void callback(
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
                std::bind(callback, std::placeholders::_1, std::placeholders::_2, socket, str));
    }
}


int main()
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
                std::bind(callback, std::placeholders::_1, std::placeholders::_2, socket, str));
        }
        
        io_context.run();
        }
    }

    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    return 0;
}