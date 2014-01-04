#ifndef FLUXCORE_BOOL_HPP
#define FLUXCORE_BOOL_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Bool : public AbstractType {
    typedef bool type;

    public:
        ~Bool() override = default;
        typeid_t getID() const override;
};

}

#endif

