#include <iostream>
#include <random>
#include "Map_vector_order_book/OrderBookVector.h"

int main() {
    OrderBookVector<2> c;
    std::vector<Element> adds;
    std::random_device rd;
    std::mt19937 g(rd());
    for (size_t i = 0; i < 10; i++) {
        adds.emplace_back(i + 1, i * 2 + 1, Side::BID);
    }
    for (size_t i = 1; i < 10; i++) {
        adds.emplace_back(i * 10, i * 5 - 4, Side::ASK);
    }
    for (auto it : c)
        std::cout << it.side << ' ' << it.price << ' ' << it.quantity << "\n";
    adds.emplace_back(3, 18, Side::BID);
    adds.emplace_back(3, 19, Side::BID);
    adds.emplace_back(10, 18, Side::ASK);
    std::shuffle(adds.begin(), adds.end(), g);
    for (auto it : adds)
        c.add(it);
    c.del(30);
    c.del(40);
    c.del(8);
    c.del(7);

    c.add(Element(30, 1, ASK));
    c.add(Element(40, 2, ASK));
    c.add(Element(7, 3, BID));
    c.add(Element(8, 4, BID));
    std::cout << "-----------\n";
    for (auto it : c)
        std::cout << it.side << ' ' << it.price << ' ' << it.quantity << "\n";

    return 0;
}
