#ifndef FLUXCORE_ABSTRACTPROVIDER_HPP
#define FLUXCORE_ABSTRACTPROVIDER_HPP

#include <cstddef>
#include <memory>

#include "segment.hpp"

namespace fluxcore {

class AbstractProvider {
    public:
        AbstractProvider() = default;
        AbstractProvider(const AbstractProvider&) = delete;
        virtual ~AbstractProvider() {}

        virtual Segment getSegment(std::size_t id) = 0;
        virtual Segment createSegment(std::size_t size) = 0;
};

typedef std::shared_ptr<AbstractProvider> provider_t;

}

#endif

