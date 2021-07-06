//
// Created by Matvey Sprikut on 05.07.2021.
//

#include "OrderBookHT.h"


void OrderBookHT::add(Element elem) {
    std::list<equal_elements> &current = elem.side == Side::ASK ? asks_list : bids_list;
    std::unordered_map<double, std::list<equal_elements>::iterator> &current_ht =
            elem.side == Side::ASK ? asks_ht : bids_ht;
    auto same_iterator = current_ht.find(elem.price);
    if (same_iterator != current_ht.end()) {
        same_iterator->second->elements.push_back(elem);
        same_iterator->second->total_price += elem.price * elem.quantity;
        same_iterator->second->total_quantity += elem.quantity;
        return;
    }
    bool f = false;
    for (auto it = current.begin(); it != current.end(); it++) {
        if (it->elements[0] > elem) {
            auto new_it = current.insert(it, elem);
            current_ht[elem.price] = new_it;
            f = true;
            break;
        }
    }
    if (!f) {
        auto new_it = current.insert(current.end(), elem);
        current_ht[elem.price] = new_it;
    }
}

void OrderBookHT::change(Element elem) {
    std::list<equal_elements> &current = elem.side == Side::ASK ? asks_list : bids_list;
    std::unordered_map<double, std::list<equal_elements>::iterator> &current_ht =
            elem.side == Side::ASK ? asks_ht : bids_ht;
    current_ht[elem.price]->total_quantity += elem.quantity - current_ht[elem.price]->elements.back().quantity;
    current_ht[elem.price]->total_price =
            current_ht[elem.price]->total_price - current_ht[elem.price]->elements.back().quantity * elem.price +
            elem.price * elem.quantity;
    current_ht[elem.price]->elements.back().quantity = elem.quantity;
}

void OrderBookHT::del(double price) {
    auto asks_it = asks_ht.find(price);
    if (asks_it != asks_ht.end()) {
        asks_it->second->total_price -= price * asks_it->second->elements.back().quantity;
        asks_it->second->total_quantity -= asks_it->second->elements.back().quantity;
        asks_it->second->elements.pop_back();
        if (asks_it->second->elements.empty()) {
            asks_list.erase(asks_it->second);
            asks_ht.erase(asks_it);
        }
        return;
    }

    auto bids_it = bids_ht.find(price);
    if (bids_it != bids_ht.end()) {
        bids_it->second->elements.pop_back();
        if (bids_it->second->elements.empty()) {
            bids_list.erase(bids_it->second);
            bids_ht.erase(bids_it);
        }
        return;
    }

}

double OrderBookHT::vwap(size_t depth) {
    double total_sum = 0;
    double total_cnt = 0;
    size_t i = 0;
    for (auto it = asks_list.begin(); it != asks_list.end() && i < depth; it++, i++) {
        total_sum += it->total_price;
        total_cnt += it->total_quantity;
    }
    i = 0;
    for (auto it = bids_list.begin(); it != bids_list.end() && i < depth; it++, i++) {
        total_sum += it->total_price;
        total_cnt += it->total_quantity;
    }
    return total_sum / total_cnt;
}

OrderBookHT::OrderBookHTIterator OrderBookHT::begin() {
    return !asks_list.empty() ? OrderBookHT::OrderBookHTIterator(asks_list.begin(), 0, &asks_list, &bids_list)
                                 : OrderBookHT::OrderBookHTIterator(bids_list.begin(), 0, &asks_list, &bids_list);
}

OrderBookHT::OrderBookHTIterator OrderBookHT::end() {
    return OrderBookHT::OrderBookHTIterator(bids_list.end(), 0, &asks_list, &bids_list);
}
