#include <benchmark/benchmark.h>
#include <memory>
#include <random>
#include "OrderBookHT.h"
#include "OrderBookVector.h"
#include "OrderBookSet.h"


template<typename T>
T CreateAdds(int n) {
    T c;
    std::random_device rd;
    std::mt19937 mt(rd());
        std::uniform_real_distribution<> distDoubleAny(100, 900);
    std::uniform_real_distribution<> distDoubleBegin(1, 100);
    std::uniform_real_distribution<> distDoubleEnd(900, 1000);
    std::uniform_int_distribution<> distInt(0, 1);
    std::uniform_int_distribution<> genStart(0, 4);

    for (int i = 0; i < n; i++) {
        Side side = distInt(mt) == 0 ? ASK : BID;
        if (genStart(mt) % 8 == 0) {
            if (side == ASK)
                c.add(Element(distDoubleBegin(mt), distDoubleAny(mt), side));
            else
                c.add(Element(distDoubleEnd(mt), distDoubleAny(mt), side));
        } else
            c.add(Element(distDoubleAny(mt), distDoubleAny(mt), side));
    }
    return c;
}


template<typename T>
static void BM_BookCreate(benchmark::State &state) {
    const int size = static_cast<int>(state.range(0));
    for (auto i : state) {
        benchmark::DoNotOptimize(CreateAdds<T>(size));
    }
    state.SetItemsProcessed(state.iterations() * size);
}

BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookVector<32>)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookVector<64>)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookVector<128>)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookVector<256>)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookVector<512>)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookHT)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_TEMPLATE(BM_BookCreate, OrderBookSet)->RangeMultiplier(4)->Range(1 << 4, 1 << 16);
BENCHMARK_MAIN();