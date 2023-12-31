#include <iostream>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)  : m_socket(std::move(socket)) { }

    void run() {
        wait_for_request();
    }
private:
    void wait_for_request() {
        auto self(shared_from_this());
        asio::async_read_until(m_socket, m_buffer, "\0", 
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)  {
                std::string data{
                    std::istreambuf_iterator<char>(&m_buffer), 
                    std::istreambuf_iterator<char>() 
                };
                std::cout << data << std::endl;
                wait_for_request();
            } else {
                std::cout << "error: " << ec << std::endl;;
            }
        });
    }
private:
    tcp::socket m_socket;
    asio::streambuf m_buffer;
};


class server
{
public:
    server(asio::io_context& io_context, short port) 
    : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
private:
    void do_accept() {
        m_acceptor.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "creating session on: " 
                    << socket.remote_endpoint().address().to_string() 
                    << ":" << socket.remote_endpoint().port() << '\n';

                std::make_shared<session>(std::move(socket))->run();
            } else {
                std::cout << "error: " << ec.message() << std::endl;
            }
            do_accept();
        });
    }
private: 
    tcp::acceptor m_acceptor;
};


int main(int argc, char* argv[])
{
    asio::io_context io_context;
    server s(io_context, 4444);
    io_context.run();

    return 0;
}