#include "init.hpp"

#include "datatypes/array.hpp"
#include "datatypes/bool.hpp"
#include "datatypes/byte.hpp"
#include "datatypes/char.hpp"
#include "datatypes/float.hpp"
#include "datatypes/int.hpp"
#include "datatypes/tuple.hpp"

using namespace fluxcore;

void fluxcore::init() {
    static bool finished = false;

    if (!finished) {
        // setup Typeregistry
        auto& r = Typeregistry::getRegistry();
        r.registerDefaultType<Array>();
        r.registerDefaultType<Bool>();
        r.registerDefaultType<Byte>();
        r.registerDefaultType<Char>();
        r.registerDefaultType<Float>();
        r.registerDefaultType<Int>();
        r.registerDefaultType<Tuple>();
    }
}

