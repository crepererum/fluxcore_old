#include "database.hpp"

#include "../datatypes/array.hpp"
#include "../datatypes/byte.hpp"
#include "../datatypes/char.hpp"
#include "../datatypes/int.hpp"

using namespace fluxcore;

struct DBAnchor {
    std::size_t tableDictIDs[2];
    std::size_t columnDictIDs[3];
    std::size_t nameDictIDs[4];
};

std::list<typeptr_t> getTableDictType() {
    return {
        std::make_shared<Int>(), // id = int
        std::make_shared<Array>(std::make_shared<Char>(), 256) // name = array<char,256>
    };
}

std::list<typeptr_t> getColumnDictType() {
    return {
        std::make_shared<Int>(), // id = int
        std::make_shared<Int>(), // table = int
        std::make_shared<Array>(std::make_shared<Byte>(), typedescrlen) // type = array<byte,$typedescrlen$>
    };
}

std::list<typeptr_t> getNameDictType() {
    return {
        std::make_shared<Int>(), // id = int
        std::make_shared<Int>(), // parent = int
        std::make_shared<Int>(), // idx = int
        std::make_shared<Array>(std::make_shared<Char>(), 256) // name = array<char,256>
    };
}

Database::Database(const provider_t& provider_) : provider(provider_) {
    // allocate anchor
    auto s = provider->createSegment(sizeof(DBAnchor));
    id = s.id();
    auto anchor = static_cast<DBAnchor*>(s.ptr());

    // allocate tableDict
    auto tableDictType = getTableDictType();
    tableDict = createAndStore(tableDictType, anchor->tableDictIDs);

    // allocate columnDict
    auto columnDictType = getColumnDictType();
    columnDict = createAndStore(columnDictType, anchor->columnDictIDs);

    // allocate nameDict
    auto nameDictType = getNameDictType();
    nameDict = createAndStore(nameDictType, anchor->nameDictIDs);

    // register all stuff
    // TODO
}

Database::Database(const provider_t& provider_, std::size_t id_) :
        provider(provider_),
        id(id_) {
    auto s = provider->getSegment(id);
    auto anchor = static_cast<DBAnchor*>(s.ptr());

    // load tableDict
    auto tableDictType = getTableDictType();
    tableDict = load(tableDictType, anchor->tableDictIDs);

    // load columnDict
    auto columnDictType = getColumnDictType();
    columnDict = load(columnDictType, anchor->columnDictIDs);

    // load nameDict
    auto nameDictType = getNameDictType();
    nameDict = load(nameDictType, anchor->nameDictIDs);
}

table_t Database::createAndStore(const std::list<typeptr_t>& types, std::size_t* mem) {
    auto result = std::make_shared<Table>(types, provider);

    for (std::size_t i = 0; i < types.size(); ++i) {
        mem[i] = result->getColumn(i)->getID();
    }

    return result;
}

table_t Database::load(const std::list<typeptr_t>& types, std::size_t* mem) {
    std::list<std::pair<typeptr_t, std::size_t>> tmp;
    std::size_t i = 0;

    for (const auto& t : types) {
        tmp.emplace_back(t, mem[i++]);
    }

    return std::make_shared<Table>(tmp, provider);
}

