#ifndef FLUXCORE_FLOAT_HPP
#define FLUXCORE_FLOAT_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Float : public AbstractType {
    public:
        ~Float() override = default;
        typeid_t getID() override;
};

}

#endif

