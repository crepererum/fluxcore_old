#ifndef FLUXCORE_SEGMENT_HPP
#define FLUXCORE_SEGMENT_HPP

#include <cstddef>

namespace fluxcore {

class Segment {
    public:
        const std::size_t id;
        void* const ptr;
        const std::size_t size;

        Segment(std::size_t id_, void* ptr_, std::size_t size_) : id(id_), ptr(ptr_), size(size_) {}
};

}

#endif

