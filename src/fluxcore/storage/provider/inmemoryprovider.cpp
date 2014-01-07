#include "inmemoryprovider.hpp"

#include <cstdlib>

using namespace fluxcore;

InmemoryProvider::~InmemoryProvider() {
    for (auto& it : segments) {
        free(it.second.ptr);
    }
}

Segment InmemoryProvider::createSegment(std::size_t size) {
    void* ptr = malloc(size);
    Segment segment{
        counter++,
        ptr,
        size
    };
    segments.insert(std::make_pair(segment.id, segment));
    return segment;
}

Segment InmemoryProvider::getSegment(std::size_t id) {
   return segments.at(id);
}

