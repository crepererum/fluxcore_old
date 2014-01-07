#include "bool.hpp"

using namespace fluxcore;

typeid_t Bool::getID() const {
    return 2;
}

std::size_t Bool::getSize() const {
    return sizeof(type);
}

std::string Bool::getName() const {
    return "bool";
}

dataptr_t Bool::createPtr(void* ptr) const {
    return std::make_shared<DataPtrTemplate<type>>(ptr);
}

dataptrconst_t Bool::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
}

