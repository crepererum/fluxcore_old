#ifndef FLUXFIX_FLATTUPLE_HPP
#define FLUXFIX_FLATTUPLE_HPP

#include <tuple>
#include <type_traits>

namespace fluxfix {

template <typename Head, typename... Tail>
class FlatTuple {
    static_assert(std::is_pod<Head>::value, "FlatTuple should be POD!");

    public:
        FlatTuple(const Head& h, const Tail&... t) : head(h), tail(t...) {}
        FlatTuple(Head&& h, Tail&&... t) : head(std::move(h)), tail(std::move(t)...) {}
        FlatTuple() = default;
        FlatTuple(const FlatTuple&) = default;
        FlatTuple(FlatTuple&&) = default;
        FlatTuple& operator=(const FlatTuple&) = default;
        FlatTuple& operator=(FlatTuple&&) = default;

        operator std::tuple<Head, Tail...>() const {
            return std::tuple_cat(std::tuple<Head>(head), static_cast<std::tuple<Tail...>>(tail));
        }

        Head head;
        FlatTuple<Tail...> tail;
};

template <typename Head>
class FlatTuple<Head> {
    static_assert(std::is_pod<Head>::value, "FlatTuple should be POD!");

    public:
        FlatTuple(const Head& h) : head(h) {}
        FlatTuple(Head&& h) : head(std::move(h)) {}
        FlatTuple() = default;
        FlatTuple(const FlatTuple&) = default;
        FlatTuple(FlatTuple&&) = default;
        FlatTuple& operator=(const FlatTuple&) = default;
        FlatTuple& operator=(FlatTuple&&) = default;

        operator std::tuple<Head>() const {
            return std::tuple<Head>(head);
        }

        Head head;
};

template <std::size_t i, typename... All>
struct GetHelper {
};

template <typename Head, typename... Tail>
struct GetHelper<0, Head, Tail...> {
    typedef Head& t;
    t x;

    GetHelper(FlatTuple<Head, Tail...>& tuple) : x(tuple.head) {}
};

template <std::size_t i, typename Head, typename... Tail>
struct GetHelper<i, Head, Tail...> : GetHelper<i - 1, Tail...> {
    typedef typename GetHelper<i - 1, Tail...>::t t;

    GetHelper(FlatTuple<Head, Tail...>& tuple) : GetHelper<i - 1, Tail...>(tuple.tail) {}
};

template <std::size_t i, typename Head, typename... Tail>
typename GetHelper<i, Head, Tail...>::t get(FlatTuple<Head, Tail...>& tuple) {
    return GetHelper<i, Head, Tail...>(tuple).x;
}

}

#endif

