#include <sstream>

#include <bandit/bandit.h>
#include <fluxcore/storage/provider/inmemoryprovider.hpp>
#include <fluxcore/storage/index.hpp>

using namespace bandit;
using namespace fluxcore;

go_bandit([](){
    describe("InmemoryProvider", [](){
        InmemoryProvider provider;
        std::map<std::size_t, Segment> record;
        std::vector<std::pair<std::size_t, std::size_t>> testPlan{
            {1,1},
            {1,1},
            {2,20},
            {3,7},
            {4,0},
            {3,0},
            {1,1},
            {5,1024},
            {5,1024},
            {5,1024},
            {3,0}
        };

        for (auto& bin : testPlan) {
            if (record.find(bin.first) == record.end()) {
                std::stringstream ss;
                ss  <<"creates segment with size "
                    << bin.second
                    << " (testID="
                    << bin.first
                    << ")";

                it(ss.str().c_str(), [&](){
                    Segment s = provider.createSegment(bin.second);

                    AssertThat(s.id(), IsGreaterThan(static_cast<std::size_t>(0)));
                    AssertThat(s.size(), Equals(bin.second));

                    record.insert(std::make_pair(bin.first, std::move(s)));
                });
            } else {
                std::stringstream ss;
                ss  << "returns correct known segment (testID="
                    << bin.first
                    << ")";

                it(ss.str().c_str(), [&](){
                    Segment good = record.at(bin.first);
                    Segment s = provider.getSegment(good.id());

                    AssertThat(s.id(), Equals(good.id()));
                    AssertThat(s.ptr(), Equals(good.ptr()));
                    AssertThat(s.size(), Equals(good.size()));
                });
            }
        }
    });

    describe("Index", [](){
        typedef std::pair<std::size_t, std::size_t> payload_t;
        provider_t provider = std::make_shared<InmemoryProvider>();
        Index<std::size_t> index(provider);
        std::vector<std::tuple<payload_t, payload_t, payload_t>> testPlan;
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({10, 1}, {10, 1}, {10, 1}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({4, 2}, {4, 2}, {10, 1}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({6, 3}, {4, 2}, {10, 1}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({11, 4}, {4, 2}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({9, 5}, {4, 2}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({1, 6}, {1, 6}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({2, 7}, {1, 6}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({3, 8}, {1, 6}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({3, 0}, {1, 6}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({3, 8}, {1, 6}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({1, 0}, {2, 7}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({7, 19}, {2, 7}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({8, 18}, {2, 7}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({2, 0}, {3, 8}, {11, 4}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({12, 14}, {3, 8}, {12, 14}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({8, 0}, {3, 8}, {12, 14}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({1, 1}, {1, 1}, {12, 14}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({2, 2}, {1, 1}, {12, 14}));
        testPlan.push_back(std::make_tuple<payload_t, payload_t, payload_t>({7, 0}, {1, 1}, {12, 14}));

        for (const auto& step : testPlan) {
            if (std::get<0>(step).second != 0) {
                std::stringstream ss;
                ss  << "Insert pair ("
                    << std::get<0>(step).first
                    << ","
                    << std::get<0>(step).second
                    << ")";

                it(ss.str().c_str(), [&](){
                    index.insert(std::get<0>(step).first, std::get<0>(step).second);

                    auto first = index.first();
                    AssertThat(first.first, Equals(std::get<1>(step).first));
                    AssertThat(first.second, Equals(std::get<1>(step).second));

                    auto last = index.last();
                    AssertThat(last.first, Equals(std::get<2>(step).first));
                    AssertThat(last.second, Equals(std::get<2>(step).second));
                });
            } else {
                std::stringstream ss;
                ss  << "Remove record "
                    << std::get<0>(step).first;

                it(ss.str().c_str(), [&](){
                    index.erase(std::get<0>(step).first);
                });
            }
        }
    });
});

