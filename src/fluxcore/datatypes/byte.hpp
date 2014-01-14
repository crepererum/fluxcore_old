#ifndef FLUXCORE_BYTE_HPP
#define FLUXCORE_BYTE_HPP

#include "../config.hpp"
#include "abstracttype.hpp"

namespace fluxcore {

class Byte : public AbstractType {
    typedef byte_t type;

    public:
        virtual ~Byte() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;
};

}

#endif

