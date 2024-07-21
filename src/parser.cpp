#include "../include/parser.hpp"
#include <cassert>
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

// parsing of block should end when the last "}" is reached for a block. 
bool Parser::parse_block(Token* current_token, BlockType block_type, StatementNode* block_node) {
    // will handle parse_if, parse_else_if, parse_else in this code block as well. 

    return true;
}

bool Parser::block_parse_completed(Token* current_token) 
{
    if (current_token->get_value() == "}") 
    {
        if(m_sBlockCounter.size() == 0) 
        {
           std::cerr << "Improper usage of scope operator" << std::endl;
           assert(false); 
        }

        m_sBlockCounter.pop();
    }

    return true;
}

// this will be creating a declation object which will be storing the required values. 
// we will typecast
bool Parser::parse_decl(Token* current_token, Node* decl_node) {
    DeclarationNode* local_node = dynamic_cast<DeclarationNode*>(decl_node); 

    if(current_token->get_value() == "fn") 
    {
       bool flag = parse_fn_decl(current_token, local_node);
    } 
    else if (current_token->get_value() == "let") 
    {
        // commented for now
        // parse_var_decl(current_token, local_node);
    }

    return true;
}

// this function is not ending
bool Parser::parse_fn_decl(Token* current_token, DeclarationNode* decl_node) {
    if(current_token->get_value() == "fn") 
    {
       if(peek()->get_type() == IDENTIFIER) {
           decl_node->decl_type = DeclarationType::FUNCTION;
           decl_node->decl_name = peek()->get_value();
           // send the parsing forward;
           parse_decl(get_next(), decl_node);
       } else {
          assert(!"Invalid syntax. Name of the function is supposed to follow after fn");
       }
    }
    else if (current_token->get_value() == "{") 
    {
        // start parsing block
        // initialize a statement block
        StatementNode* statement = new StatementNode();
        decl_node->m_stmts = statement;

        // in parse_block we will be making calling new block of memory when we are 
        // parsing new lines in our programming language. 
        parse_block(current_token, BLOCK_FUNC, statement);
    } 
    else if(current_token->get_value() == "(" && peek()->get_value() != ")") 
    {
       // this is parsing fn params.
       // Do not parse the function paramaters if there are none
       parse_fn_params(current_token, decl_node);
    } 


#if DEBUG_PARSER
    std::cout << decl_node->decl_name << std::endl;
    std::cout << decl_node->m_param_node->name << std::endl;
    std::cout << decl_node->m_param_node->m_param_type->m_eReturnType << std::endl;
#endif
    return true;
}


bool Parser::allocate_param_memory(Token* current_token) {
#if DEBUG_PARSER
    std::cout << "Came here " << current_token->get_value() << std::endl;
#endif
    if (current_token->get_value() == ",") 
    {
       return true; 
    } 
    // in case we have parameters then we to allocate memory for our parameter(s). 
    else if (current_token->get_value() == "(" && peek()->get_value() != ")") 
    {
        return true;
    }

    return false;
}

//will not be called when there are no parameters.
bool Parser::parse_fn_params(Token* current_token, DeclarationNode* fn_decl_node) 
{

    ParamNode* param = nullptr; 
    ParamNode* latest_param = nullptr, *temp = nullptr;
    if (allocate_param_memory(current_token)) 
    {
        param = new ParamNode(); 

        if (fn_decl_node->m_param_node == nullptr) 
           fn_decl_node->m_param_node = param;  
        else 
        {
            temp = fn_decl_node->m_param_node; 

            while (temp->next != nullptr)
               temp = temp->next;  

            temp->next = param; 
            // points to the last parameter; 
            latest_param = temp->next;
        }
    } 
    // if we are not allocating memory I still want to be on the last param node. 
    else 
    {
        temp = fn_decl_node->m_param_node;

        while (temp->next != nullptr) 
            temp = temp->next; 
        
        latest_param = temp;

        // added an asset here. 
        assert(latest_param != nullptr);
    }

    if (current_token->get_value() == ")") 
    {
       return true; 
    } 
    else if (current_token->get_value() == "(") 
    {
       parse_fn_params(get_next(), fn_decl_node);
    } 


    // Will be refactored for now we are working with a system that just checks if the value is a return type
    if (peek()->get_value() == ":")
       latest_param->name = get_current()->get_value();  
    else if (get_current()->get_value() == ":") 
       latest_param->m_param_type->m_eReturnType = latest_param->m_param_type->valid_return_type(peek()->get_value());; 

    parse_fn_params(get_next(), fn_decl_node);
    
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
    if(m_Index >= m_vpInputTokens.size()) 
        return nullptr;

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
