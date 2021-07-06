//
// Created by Matvey Sprikut on 05.07.2021.
//

#ifndef ORDERBOOK_ORDERBOOKVECTOR_H
#define ORDERBOOK_ORDERBOOKVECTOR_H

#include <vector>
#include <list>
#include <map>
#include "Element.h"

const int MAX_ELEMENT_TO_WATCH = 1;

enum Type {
    CHANGE, DELETE
};

// Суть решения:
// 1) Храним связный список векторов с заранее аллоцированной памятью под 2 N элементов (N некоторая константа).
// 2) Храним map с ключом = наименьшим значением в векторе, а значение - итератор, указывающий на соответсвующий вектор.
// Операция добавления: так как часто добавление происходит в начало, то посмотрим первые K векторов и проверим, что наш элемент должен быть в начале.
// Если он не нашелся, то с помощью мапа найдем вектор, в которой будем вставлять элемент. Внутри вектора, найдем куда вставлять элемент при помощи линейного поиска(можно было бинпоиск, но вроде как круче линейный по причинам cache).
// Если вектор стал размера 2 * N, разделим вершину на две вершины в списке.
// Операция изменения: аналогично добавлению с разницей, что никогда не делаем перебалансировку
// Операция удаления: аналогично добавлению и изменению. Если в какой-то момент времени две подряд идущие ноды в списке имеют суммарный размер < N, то объединим их в одну. Также не забываем поддерживать мап в валидном состоянии.
// vwap: для каждого вектора поддерживаем ответ для всех его элементов.
template<size_t N>
struct block {
    std::vector<Element> elements;
    double total_quantity, total_price;

    block() {
        elements.reserve(2 * N);
        total_quantity = 0.0;
        total_price = 0.0;
    }

    explicit block(const Element &t) {
        elements.push_back(t);
        total_price = t.price * t.quantity;
        total_quantity = t.quantity;
    }
};

struct reverse_comparator {
    bool operator()(const std::pair<Side, double> &a, const std::pair<Side, double> &b) const {
        if (a.first != b.first)
            return a.second < b.second;
        if (a.first == ASK)
            return a.second < b.second;
        return a.second > b.second;
    }
};

template<size_t N>
class OrderBookVector {
    std::list<block<N>> asks_parts, bids_parts;
    size_t asks_size{}, bids_size{};
    std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> asks_mp;
    std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> bids_mp;

    void split(typename std::list<block<N>>::iterator &insert_iterator, const Element &elem) {
        std::list<block<N>> &current = elem.side == Side::ASK ? asks_parts : bids_parts;
        std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> &current_mp =
                elem.side == Side::ASK ? asks_mp : bids_mp;
        if (insert_iterator->elements.size() == 2 * N) {
            auto new_element = new block<N>();
            for (size_t i = N; i < 2 * N; i++) {
                new_element->elements.push_back(insert_iterator->elements[i]);
            }
            auto its = current_mp.equal_range({elem.side, insert_iterator->elements.back().price});
            auto it_to_delete = its.first;
            for (auto it = its.first; it != its.second; ++it) {
                if (it->second == insert_iterator) {
                    it_to_delete = it;
                    break;
                }
            }
            current_mp.erase(it_to_delete);
            auto new_iterator = current.insert(++insert_iterator, *new_element);
            insert_iterator--;
            insert_iterator--;
            current_mp.insert({{elem.side, new_iterator->elements.back().price}, new_iterator});
            for (size_t i = N; i < 2 * N; i++)
                insert_iterator->elements.pop_back();
            current_mp.insert({{elem.side, insert_iterator->elements.back().price}, insert_iterator});
        }
    }

    void merge(typename std::list<block<N>>::iterator &to_merge, const Element &elem) {
        std::list<block<N>> &current = elem.side == Side::ASK ? asks_parts : bids_parts;
        std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> &current_mp =
                elem.side == Side::ASK ? asks_mp : bids_mp;
        auto from_merge = to_merge++;
//        std::cout << from_merge->elements.size() << ' ' << to_merge->elements.size() << "\n";
        if (to_merge == current.end() || from_merge->elements.size() + to_merge->elements.size() > N)
            return;
        auto its = current_mp.equal_range({elem.side, to_merge->elements.back().price});
        auto it_to_delete = its.first;
        for (auto it = its.first; it != its.second; ++it) {
            if (it->second == to_merge) {
                it_to_delete = it;
                break;
            }
        }
        current_mp.erase(it_to_delete);
        its = current_mp.equal_range({elem.side, from_merge->elements.back().price});
        it_to_delete = its.first;
        for (auto it = its.first; it != its.second; ++it) {
            if (it->second == to_merge) {
                it_to_delete = it;
                break;
            }
        }
        current_mp.erase(it_to_delete);
        for (size_t i = 0; i < to_merge->elements.size(); i++)
            from_merge->elements.push_back(to_merge->elements[i]);
        current.erase(to_merge);
        current_mp.insert({{elem.side, from_merge->elements.back().price}, from_merge});
    }

    bool do_operation(typename std::list<block<N>>::iterator &it, const Element &elem, Type type) {
        for (size_t i = 0; i < it->elements.size(); i++)
            if (it->elements[i].price == elem.price) {
                if (type == CHANGE)
                    it->elements[i].quantity = elem.quantity;
                else
                    it->elements.erase(it->elements.begin() + i);
                return true;
            }
        return false;
    }

public:
    void add(Element elem) {
        if (elem.side == Side::ASK)
            asks_size++;
        else
            bids_size++;
        std::list<block<N>> &current = elem.side == Side::ASK ? asks_parts : bids_parts;
        std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> &current_mp =
                elem.side == Side::ASK ? asks_mp : bids_mp;
        size_t j = 0;
        typename std::list<block<N>>::iterator insert_iterator;
        bool f = false;
        //Looking up first MAX_ELEMENT_TO_WATCH vectors
        for (auto it = current.begin(); it != current.end() && j < MAX_ELEMENT_TO_WATCH; ++it, j++) {
            if (it->elements.back() >= elem) {
                insert_iterator = it;
                f = true;
                break;
            }
        }
        if (current.size() == 0) {
            current.insert(current.begin(), block<N>());
            current.begin()->elements.push_back(elem);
            current_mp.insert({{elem.side, elem.price}, current.begin()});
            return;
        }
        //map search
        if (!f) {
            auto it = current_mp.upper_bound({elem.side, elem.price});
            if (it == current_mp.end()) {
                it--;
            }
            insert_iterator = it->second;
        }
        //If elements with equal price contains in different vectors
        while (elem >= insert_iterator->elements.back()) {
            auto tmp = insert_iterator;
            insert_iterator++;
            if (insert_iterator == current.end() ||
                tmp->elements.back().price != insert_iterator->elements.begin()->price) {
                insert_iterator--;
                break;
            }
        }
        while (insert_iterator != current.begin() && elem < insert_iterator->elements[0]) {
            auto tmp = insert_iterator;
            insert_iterator--;
            if (tmp->elements[0].price != insert_iterator->elements.back().price) {
                insert_iterator++;
                break;
            }
        }
        f = false;
        for (size_t i = 0; i < insert_iterator->elements.size(); i++) {
            if (insert_iterator->elements[i] >= elem) {
                insert_iterator->elements.insert(insert_iterator->elements.begin() + i, elem);
                f = true;
                break;
            }
        }
        //update map if need
        if (!f) {
            double pred_max_price = insert_iterator->elements.back().price;
            auto its = current_mp.equal_range({elem.side, pred_max_price});
            auto it_to_delete = its.first;
            for (auto it = its.first; it != its.second; ++it) {
                if (it->second == insert_iterator) {
                    it_to_delete = it;
                    break;
                }
            }
            current_mp.erase(it_to_delete);
            insert_iterator->elements.push_back(elem);
            current_mp.insert({{elem.side, elem.price}, insert_iterator});
        }
        split(insert_iterator, elem);
    }

// Find element with price and set new quantity for it.
    void change(Element elem) {
        std::list<block<N>> &current = elem.side == Side::ASK ? asks_parts : bids_parts;
        std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> &current_mp =
                elem.side == Side::ASK ? asks_mp : bids_mp;
        size_t j = 0;
        bool f = false;
        for (auto it = current.begin(); it != current.end() && j < MAX_ELEMENT_TO_WATCH; ++it, j++) {
            if (it->elements.back() >= elem) {
                f = true;
                do_operation(it, elem, CHANGE);
                break;
            }
        }
        if (!f) {
            auto it = current_mp.lower_bound({elem.side, elem.price})->second;
            do_operation(it, elem, CHANGE);
        }
    }

// Remove element with price equal to price.
    void del(double price) {
        for (size_t ind = 0; ind < 2; ind++) {
            std::list<block<N>> &current = ind == 0 ? asks_parts : bids_parts;
            std::multimap<std::pair<Side, double>, typename std::list<block<N>>::iterator, reverse_comparator> &current_mp =
                    ind == 0 ? asks_mp : bids_mp;
            typename std::list<block<N>>::iterator delete_iterator;
            size_t j = 0;
            bool f = false;
            bool finished = false;
            Element elem(price, 0.0, ind == 0 ? ASK : BID);
            double last_value = 0.0;
            for (auto it = current.begin(); it != current.end() && j < MAX_ELEMENT_TO_WATCH; ++it, j++) {
                if (it->elements.back() >= elem) {
                    f = true;
                    last_value = it->elements.back().price;
                    finished = do_operation(it, elem, DELETE);
                    delete_iterator = it;
                    break;
                }
            }
            if (!f) {
                auto it = current_mp.lower_bound({elem.side, elem.price});
                if (it != current_mp.end()) {
                    last_value = it->second->elements.back().price;
                    finished = do_operation(it->second, elem, DELETE);
                    delete_iterator = it->second;
                }
            }
            if (!finished)
                continue;
            if (ind == 0)
                asks_size--;
            else
                bids_size--;
            if (delete_iterator->elements.empty() || delete_iterator->elements.back().price != last_value) {
                auto its = current_mp.equal_range({elem.side, price});
                auto it_to_delete = its.first;
                for (auto it = its.first; it != its.second; ++it) {
                    if (it->second == delete_iterator) {
                        it_to_delete = it;
                        break;
                    }
                }
                current_mp.erase(it_to_delete);
                if (delete_iterator->elements.empty()) {
                    current.erase(delete_iterator);
                    break;
                } else {
                    current_mp.insert({{elem.side, delete_iterator->elements.back().price}, delete_iterator});
                }
            }
            merge(delete_iterator, elem);
            break;
        }
    }

// Calculate Volume Weighted Average Price limiting each side with top depth levels.
    double vwap(size_t depth) {
        double total_sum = 0.0;
        double total_cnt = 0.0;
        for (size_t ind = 0; ind < 2; ind++) {
            size_t i = 0;
            auto iterator = (ind == 0) ? OrderBookVectorIterator(asks_parts.begin(), 0, &asks_parts, &bids_parts)
                                       : OrderBookVectorIterator(bids_parts.begin(), 0, &asks_parts, &bids_parts);
            size_t cur_depth = (ind == 0) ? std::min(depth, asks_size) : std::min(depth, bids_size);
            while (!iterator.is_end() && i < cur_depth) {
                double price = iterator->price;
                total_sum += price * (iterator->quantity);
                total_cnt += iterator->quantity;
                while (true) {
                    iterator++;
                    if (iterator.is_end() || iterator->price != price)
                        break;
                    else {
                        total_sum += price * (iterator->quantity);
                        total_cnt += iterator->quantity;
                    }
                }
                i++;
            }
        }
        return total_sum / total_cnt;
    }

    struct OrderBookVectorIterator;

    OrderBookVectorIterator begin() {
        return asks_parts.size() != 0 ? OrderBookVectorIterator(asks_parts.begin(), 0, &asks_parts, &bids_parts)
                                      : OrderBookVectorIterator(bids_parts.begin(), 0, &asks_parts, &bids_parts);
    }

    OrderBookVectorIterator end() {
        return OrderBookVectorIterator(bids_parts.end(), 0, &asks_parts, &bids_parts);
    }
};

template<size_t N>
class OrderBookVector<N>::OrderBookVectorIterator {
    typename std::list<block<N>>::iterator order_iterator;
    size_t order_idx;
    std::list<block<N>> *asks_ptr;
    std::list<block<N>> *bids_ptr;
public:
    using list_iterator = typename std::list<block<N>>::iterator;
    using value_type = Element;
    using pointer = Element *;  // or also value_type*
    using reference = Element &;

    OrderBookVectorIterator() {};

    OrderBookVectorIterator(list_iterator it,
                            size_t idx, std::list<block<N>> *_asks_ptr, std::list<block<N>> *_bids_ptr) {
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

    OrderBookVectorIterator &operator++() {
        if (order_idx + 1 == order_iterator->elements.size()) {
            order_idx = 0;
            order_iterator++;
            if (order_iterator == asks_ptr->end())
                order_iterator = bids_ptr->begin();
        } else
            order_idx++;
        return *this;
    }

    OrderBookVectorIterator operator++(int) {
        OrderBookVectorIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool is_end() {
        return ((order_iterator == asks_ptr->end()) || (order_iterator == bids_ptr->end()));
    }

    friend bool operator==(const OrderBookVectorIterator &a, const OrderBookVectorIterator &b) {
        return a.order_iterator == b.order_iterator && a.order_idx == b.order_idx;
    }

};


#endif //ORDERBOOK_ORDERBOOKVECTOR_H
