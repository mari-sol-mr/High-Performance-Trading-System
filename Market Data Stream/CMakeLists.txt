cmake_minimum_required(VERSION 3.12)
project(alpaca-api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include/alpaca-trade-api-cpp 
../include
)
# Specify the directories for sources and headers
set(SOURCE_FILES ../include/alpaca-trade-api-cpp/alpaca/account.cpp
        ../include/alpaca-trade-api-cpp/alpaca/asset.cpp
        ../include/alpaca-trade-api-cpp/alpaca/bars.cpp
        ../include/alpaca-trade-api-cpp/alpaca/calendar.cpp
        ../include/alpaca-trade-api-cpp/alpaca/client.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/clock.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/config.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/order.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/portfolio.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/position.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/quote.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/status.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/streaming.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/trade.cpp
        # ../include/alpaca-trade-api-cpp/alpaca/watchlist.cpp 
        main.cpp)

# set(SOURCE_FILES ../include/alpaca-trade-api-cpp/alpaca/foo.cpp
# ../include/alpaca-trade-api-cpp/alpaca/account.cpp
#         main.cpp)

# set(SOURCE_FILES ../include/alpaca-trade-api-cpp/alpaca/config.cpp main.cpp)



add_executable(main ${SOURCE_FILES})

