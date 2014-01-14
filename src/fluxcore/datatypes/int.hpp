#ifndef FLUXCORE_INT_HPP
#define FLUXCORE_INT_HPP

#include <cstdint>

#include "abstracttype.hpp"

namespace fluxcore {

class Int : public AbstractType {
    typedef int64_t type;

    public:
        virtual ~Int() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;
};

}

#endif

