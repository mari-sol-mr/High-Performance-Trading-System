#include <cstdint>
#include <queue>
#include <memory>

#pragma once

enum class MarketSide
{
    Buy,
    Sell
};

enum class OrderType
{
    Limit,
    Market
};


using Price = std::int32_t; // todo: specify the size?
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

class Trade
{
public:
    Trade(Price price, OrderId askOrderId, OrderId bidOrderId, Quantity quantity)
    : price_{ price }
    , askOrderId_{ askOrderId }
    , bidOrderId_{ bidOrderId }
    , quantity_{ quantity }
    { }

    Price getPrice() const { return price_; }
    Quantity getQuantity() const {return quantity_; }
    OrderId getAskOrderId() const {return askOrderId_; }
    OrderId getBidOrderId() const {return bidOrderId_; }

private:
    Price price_;
    OrderId askOrderId_;
    OrderId bidOrderId_;
    Quantity quantity_;
};

using TradePointer = std::shared_ptr<Trade>;
using TradePointers = std::vector<TradePointer>;

class Order
{
public:
    Order(OrderId orderId, OrderType orderType, MarketSide marketSide, Price price, Quantity quantity)
    : orderId_{ orderId }
    , orderType_{ orderType }
    , marketSide_{ marketSide }
    , price_{ price }
    , initialQuantity_{ quantity }
    , remainingQuantity_{ quantity }
    { }

    MarketSide getMarketSide() const { return marketSide_;}
    Price getPrice() const { return price_;}
    Quantity getRemainingQuantity() const { return remainingQuantity_;}
    Quantity getOrderId() const { return orderId_;}
    bool isFilled() const { return remainingQuantity_ == 0;}

    void fillOrder(Quantity quantity)  { remainingQuantity_ -= quantity;}

private:

    OrderId orderId_;
    OrderType orderType_;
    MarketSide marketSide_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};


using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::vector<OrderPointer>;
using OrdersQueue = std::queue<OrderPointer>;


struct LevelInfo
{
    Price price;
    OrdersQueue ordersToBeFilled;
};




