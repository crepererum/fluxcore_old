#ifndef FLUXCORE_COMBINEDTYPE_HPP
#define FLUXCORE_COMBINEDTYPE_HPP

#include "abstracttype.hpp"

namespace fluxcore {

class CombinedPtr : public DataPtr {
    public:
        virtual dataptrconst_t getSubPtr(std::size_t i) const = 0;
};

}

#endif

