#ifndef FLUXCORE_TYPEREGISTRY_HPP
#define FLUXCORE_TYPEREGISTRY_HPP

#include <array>
#include <functional>
#include <map>

#include "../config.hpp"
#include "abstracttype.hpp"

namespace fluxcore {

class Typeregistry {
    public:
        typedef std::function<typeptr_t(typedescr_t::const_iterator&, typedescr_t::const_iterator)> parser_t;

        Typeregistry(const Typeregistry&) = delete;
        Typeregistry(Typeregistry&&) = delete;

        Typeregistry& operator=(const Typeregistry&) = delete;
        Typeregistry& operator=(Typeregistry&&) = delete;

        static Typeregistry& getRegistry();

        void registerType(typeid_t id_, const parser_t& parser_);
        typeptr_t parseType(typedescr_t::const_iterator& begin, typedescr_t::const_iterator end) const;
        typeptr_t parseType(typedescr_t::const_iterator&& begin, typedescr_t::const_iterator end) const {
            return parseType(begin, end);
        }

        template <typename T>
        void registerDefaultType() {
            registerType(T::id, T::parseDescriptor);
        }

    private:
        std::map<typeid_t, parser_t> parsers;

        Typeregistry() = default;
};

}

#endif

