#include "float.hpp"

using namespace fluxcore;

typeid_t Float::getID() const {
    return 1;
}

std::size_t Float::getSize() const {
    return sizeof(type);
}

std::string Float::getName() const {
    return "float";
}

dataptr_t Float::createPtr(void* ptr) const {
    return std::make_shared<DataPtrTemplate<type>>(ptr);
}

dataptrconst_t Float::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
}

