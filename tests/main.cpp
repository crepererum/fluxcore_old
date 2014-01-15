#include <bandit/bandit.h>

#include "fluxcore/init.hpp"

using namespace bandit;

int main(int argc, char* argv[]) {
    fluxcore::init();
    return bandit::run(argc, argv);
}

