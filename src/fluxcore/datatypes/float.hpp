#ifndef FLUXCORE_FLOAT_HPP
#define FLUXCORE_FLOAT_HPP

#include "primitivetype.hpp"

namespace fluxcore {

struct FloatHelper {
    typedef double type; // TODO select stable float type
    static constexpr typeid_t id = 1;
    static std::string getName() {
        return "float";
    }
};
typedef PrimitiveType<FloatHelper> Float;

}

#endif

