#ifndef FLUXCORE_BOOL_HPP
#define FLUXCORE_BOOL_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Bool : public AbstractType {
    typedef bool type;

    public:
        virtual ~Bool() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;
};

}

#endif

