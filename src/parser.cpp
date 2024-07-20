#include "../include/parser.hpp"
#include <vector>
#include <iostream>

#define DEBUG_PARSER true


void Parser::set_input_stream(const std::vector<Token*>& input_stream) {
    m_vpInputTokens = input_stream; 
}

// this parse will call the parse with tokens. 
bool Parser::parse() {
    parse(get_current());
    return true;
}

// this function will be recursive. 
// I want to build a tree using this class which will have the root on m_pRoot. 
// I will be building different rules for this. 
bool Parser::parse(Token* current_token) {
    int block_levels = get_current_block_level();

    // if we are not in a block
    if (block_levels == 0) {
        Parser::DeclType decl_type = check_for_declartions(current_token);

        if (decl_type != DECL_NONE) {
            DeclarationNode* decl_node = new DeclarationNode(); 
            // this is required for dynamic casting.
            // In the coming time we will have to dynamic cast the nodes to required child classes. 
            // This is where we will know what child class to cast
            decl_node->nodetype = DECLARATION;
            parse_decl(current_token, decl_node);

        } else {
        
        }
    
    } else {
        // we are inside a block;
        // can be of 3 types 
        // 1. Fn block
        // 2. Loop block
        // 3. Normal local block. 
        // 4. if and else blocks. 
    }

    return true;
}


// this will be creating a declation object which will be storing the required values. 
// we will typecast
bool Parser::parse_decl(Token* current_token, Node* decl_node) {
    DeclarationNode* local_node = dynamic_cast<DeclarationNode*>(decl_node); 
    if(current_token->get_value() == "fn") {
       parse_fn_decl(current_token, local_node);
    } else if (current_token->get_value() == "let") {
       // parse_var_decl(current_token, local_node);
    }

    return true;
}

// this function is not ending
bool Parser::parse_fn_decl(Token* current_token, DeclarationNode* decl_node) {
    if (current_token->get_value() == "(") {
        // start parsing paramaters. 
        // will need a function for this. 
        
#if DEBUG_PARSER
        std::cout << "Found Param begin" << std::endl;
#endif
        
    
    } else if (current_token->get_value() == ")") {
        // stop parsing parameters;
        
#if DEBUG_PARSER
        std::cout << "Found Param end" << std::endl;
#endif
    
    } else if (current_token->get_value() == ":") {
        // parse the return type;
        // p
#if DEBUG_PARSER
        std::cout << "Found return type" << std::endl;
#endif
    
    } else if (current_token->get_value() == "{") {
#if DEBUG_PARSER
        std::cout << "Found block begin" << std::endl;
#endif
    
    }

    parse_fn_decl(get_next(), decl_node);

    return true;
}

// So this will be checking if the current token is a declation;
// Need to add more declaration keywords
Parser::DeclType Parser::check_for_declartions(Token* current_token) {
    if (current_token->get_value() == "fn")
        return DECL_FN;

    if(current_token->get_value() == "let")
        return DECL_VAR;

    return DECL_NONE;
}

// if the current block level is greater 0 then we are in a statement block and the behaviour will be changing accordingly. 
bool Parser::get_current_block_level() {
    return m_sBlockCounter.size();
}

Token* Parser::get_current() {
    if(m_Index >= m_vpInputTokens.size()) {
        return nullptr;
    }

    return m_vpInputTokens[m_Index];
}

Token* Parser::peek() {
    if(m_Index >= m_vpInputTokens.size())
        return nullptr;

    return m_vpInputTokens[m_Index + 1];
}

// moves the current location of the token. 
Token* Parser::get_next() {
    // increment the index and check for the next one 
    ++m_Index;
    if (m_Index >= m_vpInputTokens.size()) {
       return nullptr; 
    }

    return m_vpInputTokens[m_Index];
}
