#ifndef FLUXCORE_BOOL_HPP
#define FLUXCORE_BOOL_HPP

#include "primitivetype.hpp"

namespace fluxcore {

struct BoolHelper {
    typedef bool type;
    static constexpr typeid_t id = 2;
    static std::string getName() {
        return "bool";
    }
};
typedef PrimitiveType<BoolHelper> Bool;

}

#endif

