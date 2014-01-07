#include "bool.hpp"

using namespace fluxcore;

typeid_t Bool::getID() const {
    return 2;
}

std::size_t Bool::getSize() const {
    return sizeof(type);
}

DataPtr Bool::getPtr(void* ptr) const {
    return DataPtrTemplate<type>(ptr);
}

std::string Bool::getName() const {
    return "bool";
}

