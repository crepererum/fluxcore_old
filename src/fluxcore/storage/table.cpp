#include "table.hpp"

#include "../datatypes/combinedtype.hpp"

using namespace fluxcore;

Table::Table(const std::list<typeptr_t>& columns_, const provider_t& provider_) : columns(columns_.size()) {
    std::size_t i = 0;
    for (const auto& t : columns_) {
        columns[i++] = std::make_shared<Column>(t, provider_);
    }
}

Table::Table(const std::list<std::pair<typeptr_t, std::size_t>>& columns_, const provider_t& provider_) : columns(columns_.size()) {
    std::size_t i = 0;
    for (const auto& p : columns_) {
        columns[i++] = std::make_shared<Column>(p.first, provider_, p.second);
    }
}

column_t Table::getColumn(std::size_t i) const {
    return columns.at(i);
}

void Table::addColumnRanges(std::list<std::pair<dataptrconst_t, dataptrconst_t>> ranges) {
    if (ranges.size() != columns.size()) {
        throw std::runtime_error("Number of ranges does not match the number of columns!");
    }

    auto iterRanges = ranges.cbegin();
    auto iterColumns = columns.begin();
    while (iterRanges != ranges.cend()) {
        const auto& p = *iterRanges;
        auto& c = *iterColumns;

        c->add(p.first, p.second);

        ++iterRanges;
        ++iterColumns;
    }
}

struct FreeDeleter {
    void operator()(void* ptr) {
        free(ptr);
    }
};

void Table::addRows(const dataptrconst_t& begin, const dataptrconst_t& end) {
    auto nElements = static_cast<std::size_t>(*end - *begin);

    // allocate memory
    std::list<std::unique_ptr<void, FreeDeleter>> mems; // mem guard
    std::list<std::pair<dataptrconst_t, dataptrconst_t>> ranges;
    for (const auto& c : columns) {
        auto t = c->getType();
        void* mem = malloc(t->getSize() * nElements);
        mems.push_back(std::unique_ptr<void, FreeDeleter>(mem));
        ranges.push_back(std::make_pair(t->createPtr(mem), t->createPtr(mem)));
    }

    // rearrange data
    std::shared_ptr<const CombinedPtr> rowIter = std::dynamic_pointer_cast<const CombinedPtr>(begin);
    for (std::size_t i = 0; i < nElements; ++i) {
        std::size_t cIdx = 0;
        for (auto& p : ranges) {
            auto sub = rowIter->getSubPtr(cIdx);
            memcpy(p.second->get(), sub->get(), columns[i]->getType()->getSize());
            p.second = *p.second + 1;

            ++cIdx;
        }

        rowIter = std::dynamic_pointer_cast<const CombinedPtr>(*rowIter + 1);
    }

    // add
    addColumnRanges(ranges);

    // data get freed here by unique_ptr
}

