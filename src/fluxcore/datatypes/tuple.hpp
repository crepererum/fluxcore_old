#ifndef FLUXCORE_TUPLE_HPP
#define FLUXCORE_TUPLE_HPP

#include <list>

#include "abstracttype.hpp"

namespace fluxcore {

class Tuple : public AbstractType {
    public:
        Tuple(const std::list<typeptr_t>& basetypes_);
        virtual ~Tuple() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;

    private:
        std::list<typeptr_t> basetypes;
        std::size_t size;
};

}

#endif

