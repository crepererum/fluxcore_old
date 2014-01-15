#ifndef FLUXCORE_BYTE_HPP
#define FLUXCORE_BYTE_HPP

#include "../config.hpp"
#include "primitivetype.hpp"

namespace fluxcore {

struct ByteHelper {
    typedef byte_t type;
    static constexpr typeid_t id = 5;
    static std::string getName() {
        return "byte";
    }
};
typedef PrimitiveType<ByteHelper> Byte;

}

#endif

