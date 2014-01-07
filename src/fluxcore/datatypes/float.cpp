#include "float.hpp"

using namespace fluxcore;

typeid_t Float::getID() const {
    return 1;
}

std::size_t Float::getSize() const {
    return sizeof(type);
}

DataPtr Float::getPtr(void* ptr) const {
    return DataPtrTemplate<type>(ptr);
}

std::string Float::getName() const {
    return "float";
}

