#include <bandit/bandit.h>
#include <fluxcore/datatypes/float.hpp>

using namespace bandit;
using namespace fluxcore;

go_bandit([](){
    describe("float:", [](){
        Float f;

        it("returns legal typeID", [&](){
            AssertThat(f.getID(), IsGreaterThan(0));
        });
    });
});

