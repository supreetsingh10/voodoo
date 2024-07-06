#include "../include/parser.hpp"
#include <iostream>
#include <vector>

#define DEBUG_PARSER true


void Parser::parse_tokens(std::vector<Token*> tokens) {
    std::cout << "parse_token fn" << std::endl;

#if DEBUG_PARSER
    for (auto t : tokens)
        std::cout << t->get_value() << " " << t->get_type() << std::endl;
#endif


}
