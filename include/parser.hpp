#pragma once

#include <vector>
#include "./token.hpp"

class Parser {
    public:
        void parse_tokens(std::vector<Token*> tokens);
        Token* scan_token(); 
        Token* scan_token_at_index(int index); 
        // this gets the next token
        Token* peek_token(); 

    private: 
        std::vector<Token*> m_vpTokens; 
        int m_token_number; 
};
