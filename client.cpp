#include <iostream>
#include <string>

#include <asio.hpp>

int main(int argc, char* argv[])
{
    using asio::ip::tcp;
    asio::io_context io_context;
    
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    asio::connect(socket, resolver.resolve("127.0.0.1", "4444"));
    
    std::string name;
    std::cout << "Enter your name: "; // no flush needed
    std::cin >> name;

    std::string data;
    std::cout << "Enter your message: "; // no flush needed
    std::cin >> data;


    std::string message;
    message = name + " : " + data;
    auto result = asio::write(socket, asio::buffer(message));

    std::cout << "data sent: " << data.length() << '/' << result << std::endl;

    std::error_code ec;
    socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    socket.close();

    return 0;
}