#include "../include/parser.hpp"
#include <iostream>
#include <vector>

void Parser::parse_tokens(std::vector<Token*> tokens) {
    std::cout << "parse_token fn" << std::endl;

    for (auto t : tokens)
        std::cout << t->get_value() << " " << t->get_type() << std::endl;
}
