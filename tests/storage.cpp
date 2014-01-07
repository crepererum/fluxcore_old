#include <sstream>

#include <bandit/bandit.h>
#include <fluxcore/storage/provider/inmemoryprovider.hpp>

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

                    AssertThat(s.id, IsGreaterThan(static_cast<std::size_t>(0)));
                    AssertThat(s.size, Equals(bin.second));

                    record.insert(std::make_pair(bin.first, std::move(s)));
                });
            } else {
                std::stringstream ss;
                ss  << "returns correct known segment (testID="
                    << bin.first
                    << ")";

                it(ss.str().c_str(), [&](){
                    Segment good = record.at(bin.first);
                    Segment s = provider.getSegment(good.id);

                    AssertThat(s.id, Equals(good.id));
                    AssertThat(s.ptr, Equals(good.ptr));
                    AssertThat(s.size, Equals(good.size));
                });
            }
        }
    });
});

