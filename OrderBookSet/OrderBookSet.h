//
// Created by Matvey Sprikut on 06.07.2021.
//

#ifndef ORDERBOOK_ORDERBOOK_H
#define ORDERBOOK_ORDERBOOK_H

#include "Element.h"
#include <set>

class OrderBookSet {
    std::multiset<Element> bids_set, asks_set;
public:
    // Add element with price and quantity.
    void add(Element elem);

// Find element with price and set new quantity for it.
    void change(Element elem);

// Remove element with price equal to price.
    void del(double price);

// Calculate Volume Weighted Average Price limiting each side with top depth levels.
    double vwap(size_t depth);

    struct OrderBookSetIterator;

    OrderBookSetIterator begin();

    OrderBookSetIterator end();
};

class OrderBookSet::OrderBookSetIterator {
    std::multiset<Element>::iterator order_iterator;
    std::multiset<Element> *asks_ptr;
    std::multiset<Element> *bids_ptr;
public:
    using value_type = Element;
    using pointer = Element *;  // or also value_type*
    using reference = Element &;

    OrderBookSetIterator() {};

    OrderBookSetIterator(std::multiset<Element>::iterator _order_iterator, std::multiset<Element> *_asks_ptr,
                         std::multiset<Element> *_bids_ptr) {
        asks_ptr = _asks_ptr;
        bids_ptr = _bids_ptr;
        order_iterator = _order_iterator;
    }

    value_type operator*() const {
        return *order_iterator;
    }

    OrderBookSetIterator &operator++() {
        order_iterator++;
        if (order_iterator == asks_ptr->end()) {
            order_iterator = bids_ptr->begin();
        }
        return *this;
    }

    OrderBookSetIterator operator++(int) {
        OrderBookSetIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const OrderBookSetIterator &a, const OrderBookSetIterator &b) {
        return a.order_iterator == b.order_iterator;
    }


};


#endif //ORDERBOOK_ORDERBOOK_H
