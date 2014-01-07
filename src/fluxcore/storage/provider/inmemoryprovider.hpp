#ifndef FLUXCORE_INMEMORYPROVIDER_HPP
#define FLUXCORE_INMEMORYPROVIDER_HPP

#include <map>

#include "abstractprovider.hpp"

namespace fluxcore {

class InmemoryProvider : public AbstractProvider {
    public:
        InmemoryProvider() = default;
        ~InmemoryProvider();
        Segment createSegment(std::size_t size) override;
        Segment getSegment(std::size_t id) override;

    private:
        std::map<std::size_t, Segment> segments;
        std::size_t counter = 1;
};

}

#endif

