#include <iostream>
#include "HT_list_order_book/OrderBookHT.h"

int main() {
    OrderBookHT c;
    c.add(Element(1.0, 2.0, ASK));
    c.add(Element(2.0, 2.0, ASK));
    c.add(Element(3.0, 2.0, ASK));
    c.add(Element(0.0, 2.0, ASK));
    c.add(Element(2.5, 2.0, ASK));
    c.add(Element(2.5, 4.0, ASK));

    c.add(Element(1.0, 2.0, BID));
    c.add(Element(2.0, 2.0, BID));
    c.add(Element(3.0, 2.0, BID));
    c.add(Element(0.0, 2.0, BID));
    c.add(Element(2.5, 2.0, BID));
    c.add(Element(2.5, 4.0, BID));

    c.change(Element(2.5, 1.0, BID));
    c.change(Element(2.5, 0.0, ASK));
    c.change(Element(3.0, 0.0, BID));
    c.change(Element(1.0, 0.0, ASK));

    return 0;
}
