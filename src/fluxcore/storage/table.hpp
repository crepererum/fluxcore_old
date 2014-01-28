#ifndef FLUXCORE_TABLE_HPP
#define FLUXCORE_TABLE_HPP

#include <vector>

#include "column.hpp"

namespace fluxcore {

class Table {
    public:
        Table(const std::list<typeptr_t>& columns_, const provider_t& provider_);
        Table(const std::list<std::pair<typeptr_t, std::size_t>>& columns_, const provider_t& provider_);

        column_t getColumn(std::size_t i) const;

        void addColumnRanges(std::list<std::pair<dataptrconst_t, dataptrconst_t>> ranges);
        void addRows(const dataptrconst_t& begin, const dataptrconst_t& end);

    private:
        std::vector<column_t> columns;
};

typedef std::shared_ptr<Table> table_t;

}

#endif

