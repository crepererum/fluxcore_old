#ifndef FLUXCORE_FLOAT_HPP
#define FLUXCORE_FLOAT_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Float : public AbstractType {
    typedef double type;

    public:
        ~Float() override = default;
        typeid_t getID() const override;
};

}

#endif

