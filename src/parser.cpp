#include "../include/parser.hpp"
#include "../include/ast.hpp"
#include <vector>

#define DEBUG_PARSER true


Parser::Parser() : m_Index(-1) {
}

// free the memory of the parser.
Parser::~Parser() { 
}

void Parser::set_input_stream(const std::vector<Token*>& input_stream) {
    m_vpInputTokens = input_stream; 
}

bool Parser::parse() {
    if(!parse(get_next()))
        return false; 

    parse();
    return true;
}

bool Parser::parse(Token* current_token) {
    if(current_token == nullptr)
        return false; 

    if (current_token->get_value() == "fn") {
        // allocate new memory; 
        // for declaration.
        parse_fn(current_token);
    }

    return true;
}

bool Parser::parse_fn(Token* current_token) {

    return true;
};

Token* Parser::get_current() {
    if(m_Index >= m_vpInputTokens.size()) {
        return nullptr;
    }

    return m_vpInputTokens[m_Index];
}

Token* Parser::get_next() {
    // increment the index and check for the next one 
    ++m_Index;
    if (m_Index >= m_vpInputTokens.size()) {
       return nullptr; 
    }

    return m_vpInputTokens[m_Index];
}
