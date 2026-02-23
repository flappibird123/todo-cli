#include <iostream>
#include <stdexcept>

#include "parser.hpp"
#include "time.hpp"

int main(int argc, const char *argv[]) {
    try {
        parse(argc, argv);
    } catch (std::invalid_argument &e) {
        std::cout << e.what();
        return 1;
    } catch (std::runtime_error & e) {
        std::cout << e.what();
        return 1;
    }

    return 0;
}