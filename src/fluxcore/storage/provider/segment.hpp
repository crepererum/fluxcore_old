#ifndef FLUXCORE_SEGMENT_HPP
#define FLUXCORE_SEGMENT_HPP

#include <cstddef>

namespace fluxcore {

class Segment {
    public:
        Segment(std::size_t id, void* ptr, std::size_t size) : myId(id), myPtr(ptr), mySize(size) {}
        Segment(const Segment&) = default;
        Segment(Segment&&) = default;

        Segment& operator=(const Segment&) = default;
        Segment& operator=(Segment&&) = default;

        std::size_t id() {
            return myId;
        }

        void* ptr() {
            return myPtr;
        }

        std::size_t size() {
            return mySize;
        }

    private:
        std::size_t myId;
        void* myPtr;
        std::size_t mySize;
};

}

#endif

