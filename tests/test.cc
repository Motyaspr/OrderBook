#include <gtest/gtest.h>
#include "OrderBookVector.h"
#include "OrderBookHT.h"
#include <random>
#include <set>

const double eps = 0.001;

template<size_t N>
void check(OrderBookVector<N> &x, const std::multiset<Element> &checking) {
    auto it2 = checking.begin();
    for (auto it = x.begin(); it != x.end() && it2 != checking.end(); ++it, ++it2) {
        EXPECT_EQ(*it, *it2);
//        std::cout << it->side << ' ' << it->price << ' ' << it->quantity << "\n";
    }
}

void check2(OrderBookHT &x, const std::multiset<Element> &checking) {
    auto it2 = checking.begin();
    for (auto it = x.begin(); it != x.end() && it2 != checking.end(); ++it, ++it2) {
        EXPECT_EQ(*it, *it2);
    }
}

class OrderBookCheck : public ::testing::Test {
public:
    void SetUp() {
        std::vector<Element> adds;
        std::random_device rd;
        std::mt19937 g(rd());
        for (size_t i = 0; i < 10; i++) {
            adds.emplace_back(i + 1, i * 2 + 1, Side::BID);
        }
        for (size_t i = 1; i < 10; i++) {
            adds.emplace_back(i * 10, i * 5 - 4, Side::ASK);
        }
        adds.emplace_back(3, 18, Side::BID);
        adds.emplace_back(3, 19, Side::BID);
        adds.emplace_back(10, 18, Side::ASK);
        std::shuffle(adds.begin(), adds.end(), g);


//        for (size_t i = 0; i < adds.size(); i++)
//            std::cout << "adds.emplace_back(" << adds[i].price << ", " << adds[i].quantity << ", " << adds[i].side << ")\n";
        for (const auto &elem: adds) {
            book.add(elem);
            book2.add(elem);
            checking.insert(elem);
//            check(book2, checking);
        }
    }

    std::multiset<Element> checking;
    OrderBookHT book;
    OrderBookVector<2> book2;

};

TEST_F (OrderBookCheck, orderCheck) {
    check(book2, checking);
    check2(book, checking);
}

TEST_F (OrderBookCheck, vwapStart) {
//    EXPECT_NEAR(book.vwap(1), 10, eps);
//    EXPECT_NEAR(book.vwap(2), 10.704918, eps);
//    EXPECT_NEAR(book.vwap(3), 12.678160, eps);
//    EXPECT_NEAR(book.vwap(4), 15.810344, eps);
//    EXPECT_NEAR(book.vwap(5), 19.932432, eps);
//    EXPECT_NEAR(book.vwap(10), book.vwap(100), eps);
//    EXPECT_NEAR(book2.vwap(1), 10, eps);
//    EXPECT_NEAR(book2.vwap(2), 10.704918, eps);
//    EXPECT_NEAR(book2.vwap(3), 12.678160, eps);
//    EXPECT_NEAR(book2.vwap(4), 15.810344, eps);
//    EXPECT_NEAR(book2.vwap(5), 19.932432, eps);

    EXPECT_NEAR(book2.vwap(11), book2.vwap(100), eps);
}

TEST_F(OrderBookCheck, delOrderCheck) {
    book.del(20);
    book.del(30);
    book.del(6);
    book.del(2);

    book2.del(20);
    book2.del(30);
    book2.del(6);
    book2.del(2);

    checking.erase(checking.find(Element(20, 0.0, ASK)));
    checking.erase(checking.find(Element(30, 0.0, ASK)));
    checking.erase(checking.find(Element(2, 0.0, BID)));
    checking.erase(checking.find(Element(6, 0.0, BID)));
    check(book2, checking);
    check2(book, checking);
}


TEST_F(OrderBookCheck, changeVWAP) {
    book.change(Element(10, 1, BID));
    book2.change(Element(10, 1, BID));
    EXPECT_NEAR(book.vwap(1), 10, eps);
    EXPECT_NEAR(book2.vwap(1), 10, eps);
    book.change(Element(9, 2, BID));
    book2.change(Element(9, 2, BID));
    EXPECT_NEAR(book.vwap(1), 10, eps);
    EXPECT_NEAR(book2.vwap(1), 10, eps);
    EXPECT_NEAR(book.vwap(2), 12.071428, eps);
    EXPECT_NEAR(book2.vwap(2), 12.071428, eps);
}

TEST_F(OrderBookCheck, deleteAndAdd) {
    book.del(30);
    book.del(40);
    book.del(8);
    book.del(7);

    book2.del(30);
    book2.del(40);
    book2.del(8);
    book2.del(7);

    checking.erase(checking.find(Element(30, 0.0, ASK)));
    checking.erase(checking.find(Element(40, 0.0, ASK)));
    checking.erase(checking.find(Element(7, 0.0, BID)));
    checking.erase(checking.find(Element(8, 0.0, BID)));

    check(book2, checking);
    check2(book, checking);

    book.add(Element(30, 1, ASK));
    book.add(Element(40, 2, ASK));
    book.add(Element(7, 3, BID));
    book.add(Element(8, 4, BID));

    book2.add(Element(30, 1, ASK));
    book2.add(Element(40, 2, ASK));
    book2.add(Element(7, 3, BID));
    book2.add(Element(8, 4, BID));

    checking.insert(Element(30, 1, ASK));
    checking.insert(Element(40, 2, ASK));
    checking.insert(Element(7, 3, BID));
    checking.insert(Element(8, 4, BID));

    check(book2, checking);
    check2(book, checking);
}
