#pragma once
#include "token.hpp"


// this will be our rules for the grammar which will be responsible for parsing of the code 
// we write. 
// Need to convert the code I write for the grammar to this code. 


class Parse {
    public: 
        Parse() {}
        ~Parse() {}

        bool parse_token(Token* current_token); 
}; 

