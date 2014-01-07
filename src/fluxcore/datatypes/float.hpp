#ifndef FLUXCORE_FLOAT_HPP
#define FLUXCORE_FLOAT_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Float : public AbstractType {
    typedef double type;

    public:
        virtual ~Float() override = default;
        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual DataPtr getPtr(void* ptr) const override;
        virtual std::string getName() const override;
};

}

#endif

