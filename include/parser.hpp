#pragma once

#include <vector>
#include "./token.hpp"

class Parser {
    public:
        void parse_tokens(std::vector<Token*> tokens);
};
