// #define BOOST_ASIO_HEADER_ONLY
// #define BOOST_BEAST_HEADER_ONLY
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
// #include <boost/json.hpp>
#include "json.hpp" 
#include <RingBuffer/RingBuffer.hpp>
#include "OrderBookB.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;
namespace http = beast::http;   

#include <iostream>
#include <string>

RingBuffer<int, 1024> ringBuffer;
OrderBookB ob;

int push_orders()
{
     try
    {
    // Connect
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    // Load root certificates for SSL verification
    ctx.set_default_verify_paths();
    tcp::resolver resolver{ioc};
    websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};
    
    // websocket::stream<tcp::socket> ws{ioc};
    
    auto results = resolver.resolve("stream.data.alpaca.markets", "443");
    auto ep2 = net::connect(beast::get_lowest_layer(ws), results);
    // auto ep = net::connect(ws.next_layer(), results);

        // Set SNI Hostname (required for SSL)
        if(!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), 
                                      "stream.data.alpaca.markets"))
        {
            throw beast::system_error{
                static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category()};
        }

        // SSL handshake     
        ws.next_layer().handshake(ssl::stream_base::client);

       // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");
            }));

    
    // WebSocket handshake
    ws.handshake("stream.data.alpaca.markets", "/v2/test");
    // ws.handshake("stream.data.alpaca.markets", "/v2/iex");
    std::string path_key = std::getenv("APCA-API-KEY-ID");
    std::string alpaca_secret = std::getenv("APCA-API-SECRET-KEY");

    std::string auth_msg = "{\"action\":\"auth\",\"key\":\"" + path_key + 
                       "\",\"secret\":\"" + alpaca_secret + "\"}";
    std::string sub_msg = "{\"action\":\"subscribe\",\"quotes\":[\"FAKEPACA\"]}";
    // std::string sub_msg = "{\"action\":\"subscribe\",\"trades\":[\"AAPL\"],\"bars\":[\"AAPL\"]}";

   
    ws.write(net::buffer(auth_msg));
    ws.write(net::buffer(sub_msg));



    beast::flat_buffer buffer;

    int i = 0;
    while(i < 6) {
        i++;
        std::cout << "message: " << std::to_string(i) << std::endl;
         ws.read(buffer);
        std::string message = beast::buffers_to_string(buffer.data());
        std::cout << message << std::endl;
        buffer.clear();

        if (i < 4) continue;


        // Parse JSON and write to ring buffer
       
        nlohmann::json j = nlohmann::json::parse(message);
        int bid_price = j[0]["bp"];     
        int bid_size = j[0]["bs"];    
        int ask_price = j[0]["ap"];     
        int ask_size = j[0]["as"];      


        ringBuffer.push(bid_price);
        ringBuffer.push(bid_size);
        ringBuffer.push(ask_price);
        ringBuffer.push(ask_size);

       
    }

    ws.close(websocket::close_code::normal);
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    cout << "end  of producer\n";
    return EXIT_SUCCESS;
}

void update_orderbook()
{
    int bp;
    int sp;
    int orders = 0;
    auto consume = [i = 0, sp, bp, &orders](int item) mutable {
        std::cout << "item:  " << item << "i: " << i << "\n";
        ob.submitOrderRequest(OrderType::Limit, MarketSide::Sell, 5, 2);
        ob.submitOrderRequest(OrderType::Limit, MarketSide::Buy, 5, 1);
        if (i == 0) bp = item;
        else if (i == 1)  ob.submitOrderRequest(OrderType::Limit, MarketSide::Buy, bp, item);
        else if (i == 2) sp = item;
        else 
        {
            ob.submitOrderRequest(OrderType::Limit, MarketSide::Sell, sp, item);
            i = 0;
            orders++;
            cout << "orders: " << orders << "\n";
        }
        i++;
    };
   
    while (orders < 1)
    {
       
        ringBuffer.consume_one(consume);
    }
    cout << "end  of consumer\n";
}

int main() {
   
    std::thread producer(push_orders);
    std::thread consumer(update_orderbook);
    
    
    producer.join();
    consumer.join();

    ob.printTradeHistory();

    return EXIT_SUCCESS;
}