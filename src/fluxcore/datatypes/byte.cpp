#include "byte.hpp"

using namespace fluxcore;

typeid_t Byte::getID() const {
    return 5;
}

std::size_t Byte::getSize() const {
    return sizeof(type);
}

std::string Byte::getName() const {
    return "byte";
}

dataptr_t Byte::createPtr(void* ptr) const {
    return std::make_shared<DataPtrTemplate<type>>(ptr);
}

dataptrconst_t Byte::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
}

