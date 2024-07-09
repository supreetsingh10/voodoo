#pragma once

#include <stack>
#include <vector>
#include "./token.hpp"


class Parser {
    public: 
        Parser(); 
        ~Parser(); 

        Token* get_next(); 
        Token* get_current(); 

        void set_input_stream(const std::vector<Token*>& input_stream); 

        bool parse(); 
        bool parse_fn(Token* current_token);

    private: 
        bool parse(Token* current_token); 

        int m_Index; 
        std::vector<Token*> m_vpInputTokens; 

        std::stack<int> m_sBlockCounter;
};
