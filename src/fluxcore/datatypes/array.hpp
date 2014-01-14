#ifndef FLUXCORE_ARRAY_HPP
#define FLUXCORE_ARRAY_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Array : public AbstractType {
    public:
        Array(typeptr_t& basetype_, std::size_t size_);
        virtual ~Array() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;

    private:
        typeptr_t basetype;
        std::size_t size;
};

}

#endif

