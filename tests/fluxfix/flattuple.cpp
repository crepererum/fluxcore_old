#include <bandit/bandit.h>

#include <cstring>
#include <memory>
#include <type_traits>

#include <fluxfix/flattuple.hpp>

using namespace bandit;
using namespace fluxfix;

void test_flattuple() {
    go_bandit([](){
        describe("FlatTuple", [](){
            typedef FlatTuple<int, float, char> tuple_t;
            tuple_t t{5, 1.3, 'c'};

            it("is POD", [](){
                AssertThat(std::is_pod<tuple_t>::value, Equals(true));
            });

            it("provides get<I> methods", [&](){
                AssertThat(get<0>(t), Equals(5));
                AssertThat(get<1>(t), EqualsWithDelta(1.3, 0.001));
                AssertThat(get<2>(t), Equals('c'));
            });

            it("is struct compatible", [&](){
                struct struct_t {
                    int i;
                    float f;
                    char c;
                };
                struct_t s;

                AssertThat(sizeof(tuple_t), Equals(sizeof(struct_t)));

                memcpy(&s, &t, sizeof(tuple_t));
                AssertThat(s.i, Equals(get<0>(t)));
                AssertThat(s.f, Equals(get<1>(t)));
                AssertThat(s.c, Equals(get<2>(t)));
            });

            it("is std::tuple compatible", [&](){
                auto t2 = static_cast<std::tuple<int, float, char>>(t);
                AssertThat(std::get<0>(t2), Equals(get<0>(t)));
                AssertThat(std::get<1>(t2), EqualsWithDelta(get<1>(t), 0.001));
                AssertThat(std::get<2>(t2), Equals(get<2>(t)));
            });
        });
    });
}

