#include <bandit/bandit.h>

#include "all.hpp"

#include <fluxcore/init.hpp>

using namespace bandit;

int main(int argc, char* argv[]) {
    fluxcore::init();

    test_fluxfix();
    test_fluxcore();

    return bandit::run(argc, argv);
}

