#include "column.hpp"

#include <cstring>

using namespace fluxcore;

Column::Column(const typeptr_t& type_, const provider_t& provider_) :
        type(type_),
        provider(provider_),
        index(provider_) {
}

Column::Column(const typeptr_t& type_, const provider_t& provider_, std::size_t id_) :
        type(type_),
        provider(provider_),
        index(provider_, id_) {}

std::size_t Column::getID() const {
    return index.getID();
}

typeptr_t Column::getType() const {
    return type;
}

void Column::add(const dataptrconst_t& begin, const dataptrconst_t& end) {
    // prepare plain memory operation
    std::size_t nElements = (*begin - *end);
    std::size_t nBytes = nElements * type->getSize();

    // copy data to new segment
    Segment s = provider->createSegment(nBytes);
    memcpy(s.ptr(), begin->get(), nBytes);

    // add segment to index
    std::size_t pos = 0;
    if (!index.empty()) {
        pos = index.last().first;
    }
    index.insert(pos + nElements, s.id());
}


