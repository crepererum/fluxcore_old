#include <list>
#include <set>
#include <utility>

#include <bandit/bandit.h>

#include <fluxcore/datatypes/bool.hpp>
#include <fluxcore/datatypes/float.hpp>

using namespace bandit;
using namespace fluxcore;

typedef std::shared_ptr<AbstractType> typeptr_t;
typedef std::list<std::pair<typeptr_t, std::string>> typelist_t;

template <typename T>
void addType(std::string name, typelist_t& list) {
    list.push_back(std::make_pair<typeptr_t, std::string>(std::make_shared<T>(), std::move(name)));
}

go_bandit([](){
    describe("datatypes:", [](){
        std::list<std::pair<typeptr_t, std::string>> all;
        std::set<std::size_t> ids;
        
        it("is possible to initialize all", [&](){
            addType<Float>("float", all);
            addType<Bool>("bool", all);
        });

        for (auto& p : all) {
            describe(p.second.c_str(), [&](){
                typeptr_t& t = p.first;

                it("returns a legal typeID", [&](){
                    auto id = t->getID();
                    AssertThat(id, IsGreaterThan(0));
                    ids.insert(id);
                });
            });
        }

        it("has a unique ID collection", [&](){
            AssertThat(ids.size(), Equals(all.size()));
        });
    });
});

