//
// Created by Matvey Sprikut on 05.07.2021.
//

#ifndef ORDERBOOK_ORDERBOOKHT_H
#define ORDERBOOK_ORDERBOOKHT_H

#include "Element.h"
#include <list>
#include <unordered_map>
#include <vector>

struct equal_elements {
    std::vector<Element> elements;
    double total_quantity, total_price;

    equal_elements(const Element &t) {
        elements.push_back(t);
        total_price = t.price * t.quantity;
        total_quantity = t.quantity;
    }
};

class OrderBookHT {
    std::list<equal_elements> bids_list, asks_list;
    std::unordered_map<double, std::list<equal_elements>::iterator> bids_ht, asks_ht;

public:
// Add element with price and quantity.
    void add(Element elem);

// Find element with price and set new quantity for it.
    void change(Element elem);

// Remove element with price equal to price.
    void del(double price);

// Calculate Volume Weighted Average Price limiting each side with top depth levels.
    double vwap(size_t depth);

    struct OrderBookHTIterator;

    OrderBookHTIterator begin();

    OrderBookHTIterator end();
};

class OrderBookHT::OrderBookHTIterator {
    std::list<equal_elements>::iterator order_iterator;
    size_t order_idx;
    std::list<equal_elements> *asks_ptr;
    std::list<equal_elements> *bids_ptr;
public:
    using list_iterator = std::list<equal_elements>::iterator;
    using value_type = Element;
    using pointer = Element *;  // or also value_type*
    using reference = Element &;

    OrderBookHTIterator() {};

    OrderBookHTIterator(list_iterator it,
                        size_t idx, std::list<equal_elements> *_asks_ptr, std::list<equal_elements> *_bids_ptr) {
        order_iterator = it;
        order_idx = idx;
        asks_ptr = _asks_ptr;
        bids_ptr = _bids_ptr;
    }

    reference operator*() const {
        return order_iterator->elements[order_idx];
    }

    pointer operator->() {
        return &order_iterator->elements[order_idx];
    }

    OrderBookHTIterator &operator++() {
        if (order_idx + 1 == order_iterator->elements.size()) {
            order_idx = 0;
            order_iterator++;
            if (order_iterator == asks_ptr->end())
                order_iterator = bids_ptr->begin();
        } else
            order_idx++;
        return *this;
    }

    OrderBookHTIterator operator++(int) {
        OrderBookHTIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const OrderBookHTIterator &a, const OrderBookHTIterator &b) {
        return a.order_iterator == b.order_iterator && a.order_idx == b.order_idx;
    }

};


#endif //ORDERBOOK_ORDERBOOKHT_H
