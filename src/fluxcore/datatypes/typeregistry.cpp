#include "typeregistry.hpp"

#include <stdexcept>

using namespace fluxcore;

Typeregistry& Typeregistry::getRegistry() {
    static Typeregistry singelton;
    return singelton;
}

void Typeregistry::registerType(typeid_t id_, const parser_t& parser_) {
    parsers.emplace(std::make_pair(id_, parser_));
}

typeptr_t Typeregistry::parseType(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end) const {
    if (begin == end) {
        throw std::runtime_error("Illegal type descriptor!");
    }

    return parsers.at(*begin)(begin, end);
}

