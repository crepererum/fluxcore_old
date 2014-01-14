#ifndef FLUXCORE_CHAR_HPP
#define FLUXCORE_CHAR_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class Char : public AbstractType {
    typedef char32_t type;

    public:
        virtual ~Char() override = default;

        virtual typeid_t getID() const override;
        virtual std::size_t getSize() const override;
        virtual std::string getName() const override;

        virtual dataptr_t createPtr(void* ptr) const override;
        virtual dataptrconst_t createPtr(const void* ptr) const override;
};

}

#endif

