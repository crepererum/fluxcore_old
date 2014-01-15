#ifndef FLUXCORE_TUPLE_HPP
#define FLUXCORE_TUPLE_HPP

#include <list>

#include "../config.hpp"
#include "abstracttype.hpp"
#include "typeregistry.hpp"

namespace fluxcore {

class Tuple : public AbstractType {
    public:
        static constexpr typeid_t id = 7;

        Tuple(const std::list<typeptr_t>& basetypes_);
        virtual ~Tuple() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;

        virtual void generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const;
        static typeptr_t parseDescriptor(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end);

    private:
        std::list<typeptr_t> basetypes;
        tupleSize_t size;
};

}

#endif

