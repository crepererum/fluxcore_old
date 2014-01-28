#ifndef FLUXCORE_COLUMN_HPP
#define FLUXCORE_COLUMN_HPP

#include "../datatypes/abstracttype.hpp"
#include "provider/abstractprovider.hpp"
#include "index.hpp"

namespace fluxcore {

class Column {
    public:
        Column(const typeptr_t& type_, const provider_t& provider_);
        Column(const typeptr_t& type_, const provider_t& provider_, std::size_t id_);

        std::size_t getID() const;
        typeptr_t getType() const;

        void add(const dataptrconst_t& begin, const dataptrconst_t& end);

    private:
        typeptr_t type;
        provider_t provider;
        Index<std::size_t, 16> index;
};

typedef std::shared_ptr<Column> column_t;

}

#endif

