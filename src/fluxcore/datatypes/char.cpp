#include "char.hpp"

using namespace fluxcore;

typeid_t Char::getID() const {
    return 6;
}

std::size_t Char::getSize() const {
    return sizeof(type);
}

std::string Char::getName() const {
    return "char";
}

dataptr_t Char::createPtr(void* ptr) const {
    return std::make_shared<DataPtrTemplate<type>>(ptr);
}

dataptrconst_t Char::createPtr(const void* ptr) const {
    // black voodoo!
    return std::make_shared<DataPtrTemplate<type>>(const_cast<void*>(ptr));
}

