#include <array>
#include <iostream>
#include <tuple>

#include <hydrogen.h>

#include "sfc.h"


int main(int argc, char *argv[]) {
    if (hydro_init() != 0) {
        std::cerr << "Failed to init hydro library" << std::endl;
        return EXIT_FAILURE;
    }

    struct args t;
    int err;
    std::tie(t, err) = parse_args(argc, argv);
    if (err)
        return err;

    switch (t.op) {
    case args::ENCRYPT:
        return encrypt(std::cin, std::cout, t.pw);
        break;
    case args::DECRYPT:
        return decrypt(std::cin, std::cout, t.pw);
        break;
    default:
        return EXIT_FAILURE;
    }
}
