#include "int.hpp"

using namespace fluxcore;

typeid_t Int::getID() const {
    return 4;
}

std::size_t Int::getSize() const {
    return sizeof(type);
}

std::string Int::getName() const {
    return "int";
}

dataptr_t Int::createPtr(void* ptr) const {
    return std::make_shared<DataPtrTemplate<type>>(ptr);
}

dataptrconst_t Int::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
}

