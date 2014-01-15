#include <list>
#include <set>
#include <utility>

#include <bandit/bandit.h>

#include <fluxcore/datatypes/bool.hpp>
#include <fluxcore/datatypes/float.hpp>
#include <fluxcore/datatypes/int.hpp>
#include <fluxcore/datatypes/byte.hpp>
#include <fluxcore/datatypes/char.hpp>
#include <fluxcore/datatypes/array.hpp>
#include <fluxcore/datatypes/tuple.hpp>

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
            addType<Int>("int", all);
            addType<Byte>("byte", all);
            addType<Char>("char", all);

            all.push_back(std::make_pair<typeptr_t, std::string>(std::make_shared<Array>(all.begin()->first, 10), "array<bool,10>"));

            std::list<typeptr_t> tupleList;
            tupleList.push_back(std::make_shared<Float>());
            tupleList.push_back(std::make_shared<Bool>());
            tupleList.push_back(std::make_shared<Char>());
            all.push_back(std::make_pair<typeptr_t, std::string>(std::make_shared<Tuple>(tupleList), "tuple<float,bool,char>"));
        });

        for (auto& p : all) {
            describe(p.second.c_str(), [&](){
                typeptr_t& t = p.first;

                it("returns a legal typeID", [&](){
                    auto id = t->getID();
                    AssertThat(id, IsGreaterThan(0));
                    ids.insert(id);
                });

                it("can generate and parse a typedescriptor", [&](){
                    typedescr_t descriptor;
                    t->generateDescriptor(descriptor.begin(), descriptor.end());

                    typeptr_t t2 = Typeregistry::getRegistry().parseType(descriptor.cbegin(), descriptor.cend());
                    AssertThat(t2->getID(), Equals(t->getID()));
                    AssertThat(t2->getSize(), Equals(t->getSize()));
                    AssertThat(t2->getName(), Equals(t->getName()));
                });
            });
        }

        it("has a unique ID collection", [&](){
            AssertThat(ids.size(), Equals(all.size()));
        });
    });
});

