#ifndef FLUXCORE_INT_HPP
#define FLUXCORE_INT_HPP

#include "../config.hpp"
#include "primitivetype.hpp"

namespace fluxcore {

struct IntHelper {
    typedef int_t type;
    static constexpr typeid_t id = 4;
    static std::string getName() {
        return "int";
    }
};
typedef PrimitiveType<IntHelper> Int;

}

#endif

