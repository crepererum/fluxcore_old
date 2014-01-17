#ifndef FLUXCORE_ARRAY_HPP
#define FLUXCORE_ARRAY_HPP

#include "abstracttype.hpp"
#include "typeregistry.hpp"

namespace fluxcore {

class Array : public AbstractType {
    public:
        static constexpr typeid_t id = 3;

        Array(const typeptr_t& basetype_, arraySize_t size_);
        Array(typeptr_t&& basetype_, arraySize_t size_);
        virtual ~Array() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;

        virtual void generateDescriptor(typedescr_t::iterator& begin, typedescr_t::iterator end) const;
        static typeptr_t parseDescriptor(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end);

    private:
        typeptr_t basetype;
        arraySize_t size;
};

}

#endif

