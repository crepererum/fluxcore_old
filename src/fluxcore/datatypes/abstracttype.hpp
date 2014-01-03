#ifndef FLUXCORE_ABSTRACTTYPE_HPP
#define FLUXCORE_ABSTRACTTYPE_HPP

#include "../config.hpp"

namespace fluxcore {

class AbstractType {
    public:
        virtual ~AbstractType() = default;
        virtual typeid_t getID() = 0;
};

}

#endif

