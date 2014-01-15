#ifndef FLUXCORE_CHAR_HPP
#define FLUXCORE_CHAR_HPP

#include "../config.hpp"
#include "primitivetype.hpp"

namespace fluxcore {

struct CharHelper {
    typedef char_t type;
    static constexpr typeid_t id = 6;
    static std::string getName() {
        return "char";
    }
};
typedef PrimitiveType<CharHelper> Char;

}

#endif

