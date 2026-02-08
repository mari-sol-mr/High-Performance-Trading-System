
#include "OrderBookTypes.h"
#include <vector>
#include <map>

// Generic max heap and min heap templates
#include <queue>
#include <functional>
#include <thread>



class OrderBookB
{
public:
    OrderBookB()
    : totalOrders_ { 0 }
    { 
        orderHistory_.reserve(100000000);
    }

    // todo: benchmark test: compare single vs multi-threaded: do loggin stuff on a separate thread.
    //todo: debugger for cpp in vscode??
    void submitOrderRequest(OrderType orderType, MarketSide marketSide, Price price, Quantity quantity)
    {
        OrderPointer newOrder = createOrder(orderType, marketSide, price, quantity);

        logOrder(newOrder);

        matchOrder(newOrder);
    }

    void submitOrderRequest(OrderType orderType, MarketSide marketSide, Quantity quantity)
    {
        std::cout << "market order submitted\n";

        // todo: what if bids and asks are empty?
        Price bestPrice = marketSide == MarketSide::Buy ? unfilledAsks_.begin()->first : unfilledBids_.begin()->first;
        std::cout << "best price: " << bestPrice << "\n";
        submitOrderRequest(orderType, marketSide, bestPrice, quantity);
    }

    OrderPointer createOrder(OrderType orderType, MarketSide marketSide, Price price, Quantity quantity)
    {
        OrderId orderId = generateOrderId();

        return std::make_shared<Order>(orderId, orderType, marketSide, price, quantity);
    }

    OrderId generateOrderId()
    {
        return ++totalOrders_;
    }

    void logOrder(OrderPointer order)
    {
        orderHistory_.push_back(order);
    }

    void logTrade(TradePointer trade)
    {
        tradeHistory_.push_back(trade);
    }

    TradePointer executeTrade(OrderPointer order1, OrderPointer order2)
    {
        Quantity tradeQuantity = std::min(order1->getRemainingQuantity(), order2->getRemainingQuantity());

        order1->fillOrder(tradeQuantity);
        order2->fillOrder(tradeQuantity);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        return std::make_shared<Trade>(order1->getPrice(), order2->getOrderId(), order1->getOrderId(), tradeQuantity);
    }

    void matchOrder(OrderPointer order)
    {
        bool matchAvailable = canMatch(order);

        if (!matchAvailable){
            queueOrder(order);
            return;
        }
        
        OrdersQueue* unfilledOrders = MarketSide::Buy == order->getMarketSide() ? &unfilledAsks_[order->getPrice()] : &unfilledBids_[order->getPrice()];

        while(!order->isFilled() && matchAvailable)
        {
            OrderPointer bestMatch = unfilledOrders->front();

            TradePointer trade = executeTrade(order, bestMatch);
            
            logTrade(trade); // todo: maybe there is some way to optimize this and reserve x space?

            if (bestMatch->isFilled())
            {
                unfilledOrders->pop();
            }

            if (unfilledOrders->empty())
            {
                matchAvailable = false;
                if (bestMatch->getMarketSide() == MarketSide::Buy)
                    unfilledBids_.erase(unfilledBids_.begin());
                else
                    unfilledAsks_.erase(unfilledAsks_.begin());
            }
        }

        if (!order->isFilled())
        {
            queueOrder(order);
        }

        std::cout << "\n\n\n";
    }

    bool canMatch(OrderPointer order)
    {
        if (order->getMarketSide() == MarketSide::Buy) 
            return unfilledAsks_.contains(order->getPrice()) && !unfilledAsks_[order->getPrice()].empty();
        else 
            return unfilledBids_.contains(order->getPrice()) && !unfilledBids_[order->getPrice()].empty();
    }

    void queueOrder(OrderPointer order)
    {
        if (order->getMarketSide() == MarketSide::Buy) 
            unfilledBids_[order->getPrice()].push(order);
        else  
            unfilledAsks_[order->getPrice()].push(order);
    }

    void printTradeHistory()
    {
        std::cout << "printing hist\n";
        for (const auto& trade : tradeHistory_)
        {
            std::cout << "Traded quantity " << trade->getQuantity() << " at price " << trade->getPrice()
            << " with orders " << trade->getAskOrderId() << " and " << trade->getBidOrderId() << "\n"; 
        }
    }


private:
    int totalOrders_;
    OrderPointers orderHistory_; // todo: refactor to orders list
    TradePointers tradeHistory_; // todo: refactor to trades list
    std::map<Price, OrdersQueue, std::less<Price>> unfilledAsks_;
    std::map<Price, OrdersQueue, std::greater<Price>> unfilledBids_;
};


    

// g++ OrderBookB.cpp -o OrderBookB