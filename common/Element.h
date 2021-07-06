//
// Created by Matvey Sprikut on 02.07.2021.
//

#ifndef ORDERBOOK_ELEMENT_H
#define ORDERBOOK_ELEMENT_H

enum Side {
    BID, ASK
};

struct Element {
    double price;
    double quantity;
    Side side;

    Element(double _price, double _quantity, Side _side) : price(_price), quantity(_quantity), side(_side) {};

    Element(Element const &other) {
        price = other.price;
        quantity = other.quantity;
        side = other.side;
    }

    friend bool operator==(Element const &a, Element const &b) {
        return a.side == b.side && a.price == b.price;
    }

    friend bool operator<(Element const &a, Element const &b) {
        if (a.side == b.side) {
            if (a.side == Side::BID)
                return a.price > b.price;
            else
                return a.price < b.price;
        }
        return a.side > b.side;
    }

    friend bool operator>(Element const &a, Element const &b) {
        if (a.side == b.side) {
            if (a.side == Side::BID)
                return a.price < b.price;
            else
                return a.price > b.price;
        }
        return a.side < b.side;
    }

    friend bool operator>=(Element const &a, Element const &b) {
        if (a.side == b.side) {
            if (a.side == Side::BID)
                return a.price <= b.price;
            else
                return a.price >= b.price;
        }
        return a.side <= b.side;
    }
};


#endif //ORDERBOOK_ELEMENT_H
