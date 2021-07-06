//
// Created by Matvey Sprikut on 06.07.2021.
//

#include "OrderBookSet.h"

void OrderBookSet::add(Element elem) {
    if (elem.side == ASK)
        asks_set.insert(elem);
    else
        bids_set.insert(elem);
}

void OrderBookSet::change(Element elem) {
    std::multiset<Element> &current_set = elem.side == ASK ? asks_set : bids_set;
    current_set.erase(current_set.find(elem));
    current_set.insert(elem);
}

void OrderBookSet::del(double price) {
    for (size_t ind = 0; ind < 2; ind++) {
        std::multiset<Element> &current_set = (ind == 0) ? asks_set : bids_set;
        Element x(price, 0, ind == 0 ? ASK : BID);
        auto it = current_set.find(x);
        if (it != current_set.end()) {
            current_set.erase(it);
            break;
        }
    }

}

double OrderBookSet::vwap(size_t depth) {
    double total_sum = 0;
    double total_cnt = 0;
    for (size_t ind = 0; ind < 2; ind++) {
        std::multiset<Element> &current_set = (ind == 0) ? asks_set : bids_set;
        auto it = current_set.begin();
        size_t i = 0;
        while (it != current_set.end() && i < depth) {
            double price = it->price;
            while (it != current_set.end() && price == it->price) {
                total_sum += it->price * it->quantity;
                total_cnt += it->quantity;
                it++;
            }
            i++;
        }
    }
    return total_sum / total_cnt;
}

OrderBookSet::OrderBookSetIterator OrderBookSet::begin() {
    return asks_set.empty() ? OrderBookSet::OrderBookSetIterator(bids_set.begin(), &asks_set, &bids_set) :
           OrderBookSet::OrderBookSetIterator(asks_set.begin(), &asks_set, &bids_set);
}

OrderBookSet::OrderBookSetIterator OrderBookSet::end() {
    return OrderBookSet::OrderBookSetIterator(bids_set.end(), &asks_set, &bids_set);
}

