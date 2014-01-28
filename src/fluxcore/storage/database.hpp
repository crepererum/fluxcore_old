#ifndef FLUXCORE_DATABASE_HPP
#define FLUXCORE_DATABASE_HPP

#include "table.hpp"

namespace fluxcore {

class Database {
    public:
        Database(const provider_t& provider_);
        Database(const provider_t& provider_, std::size_t id_);

        std::size_t getID() const;

    private:
        provider_t provider;
        std::size_t id;
        table_t tableDict;
        table_t columnDict;
        table_t nameDict;

        table_t createAndStore(const std::list<typeptr_t>& types, std::size_t* mem);
        table_t load(const std::list<typeptr_t>& types, std::size_t* mem);

};

}

#endif

