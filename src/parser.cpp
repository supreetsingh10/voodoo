#include "../include/parser.hpp"
#include <bits/fs_fwd.h>
#include <cassert>
#include <vector>
#include <iostream>


#define DEBUG_PARSER true


void Parser::set_input_stream(const std::vector<Token*>& input_stream) 
{
    m_vpInputTokens = input_stream; 
}

// this parse will call the parse with tokens. 
bool Parser::parse() 
{
    while (m_Index < m_vpInputTokens.size()) 
    {
        bool success_parse = parse(get_current());

        if (!success_parse)
            return false;
        else
            m_Index++;
    }


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

        if (decl_type != DECL_NONE) 
        {
            DeclarationNode* decl_node = new DeclarationNode(); 
            // this is required for dynamic casting.
            // In the coming time we will have to dynamic cast the nodes to required child classes. 
            // This is where we will know what child class to cast
            decl_node->nodetype = DECLARATION;
            m_bStartNodeSet = parse_decl(current_token, decl_node);

            if(!m_bStartNodeSet) 
            {
               m_StartNode = decl_node;
               m_bStartNodeSet = true;
#if DEBUG_PARSER
               DeclarationNode *test = dynamic_cast<DeclarationNode*>(m_StartNode);
               StatementNode* st = test->m_stmts;
               while (!st) 
               {
                   std::cout << "FN STMT DECLS " << st->m_stmt_decl->decl_name << std::endl;
                   st = st->next;
               }
#endif
            }
        }
    }

    return true;
}

Token* Parser::skip_and_get(const size_t& skip_num) 
{
    assert(skip_num + m_Index < m_vpInputTokens.size());
    m_Index += skip_num;

    return  m_vpInputTokens[m_Index];
}

bool Parser::allocate_stmt_memory(Token* current_stmt_token) 
{
    if (current_stmt_token->get_value() == ";") 
       return true; 

    return false;
}

// parsing of block should end when the last "}" is reached for a block. 
bool Parser::parse_block(Token* current_token, BlockType block_type, StatementNode* block_node) 
{
    if (current_token->get_value() == "}") 
    {
       block_level_updater(current_token);
       return true; 
    }


    StatementNode* temp_node = nullptr;

    if(allocate_stmt_memory(current_token)) 
    {
       // this will chain our statement blocks. 
       temp_node = new StatementNode();  
       block_node->next = temp_node; 
    }  
    else
        temp_node = block_node;

    DeclType local_decl_type = check_for_declartions(current_token);
    if (local_decl_type != DECL_NONE) 
    {
        if (local_decl_type == DECL_FN) 
        {
           std::cerr << "Cannot declare functions inside a block" << std::endl;
           assert(false);
           return false; 
        } 
        else if (local_decl_type == DECL_VAR) 
        {
            DeclarationNode* local_decl_node = new DeclarationNode();

            if (temp_node == nullptr) 
            {
                std::cerr << "Statement node happens to be null" << std::endl; 
                assert(false);
            }


            if (temp_node->m_stmt_decl == nullptr) 
            {
                temp_node->m_stmt_decl = local_decl_node;
            } 
            else 
            {
                // we will always need a latest node.
                DeclarationNode* latest_decl = nullptr;

                while (latest_decl->next != nullptr) 
                    latest_decl = latest_decl->next;

                latest_decl->next = local_decl_node;
            }

            if(!parse_var_decl(current_token, local_decl_node)) 
            {
                std::cerr << "Failed to parse the local variable " << current_token->get_value() << std::endl; 
                assert(false);
            }

        }

    }


    parse_block(get_next(), block_type, temp_node);
    return true;
}


bool Parser::block_parse_completed(Token* current_token) 
{
    if (current_token->get_value() == "}") 
    {
        if(m_sBlockStack.size() == 0) 
        {
           std::cerr << "Improper usage of scope operator" << std::endl;
           assert(false); 
           return false;
        }

        m_sBlockStack.pop();
    }

    return true;
}


void Parser::block_level_updater(Token* current_token) 
{
    if (current_token->get_value() == "{") 
        m_sBlockStack.push('{');
    else if (current_token->get_value() == "}") 
    {
        if (m_sBlockStack.size() == 0) {
            std::cerr << "Improper usage of }" << std::endl;
            assert(false);
        }

        m_sBlockStack.pop();
    }
}


// this will be creating a declation object which will be storing the required values. 
// we will typecast
bool Parser::parse_decl(Token* current_token, Node* decl_node) {
    DeclarationNode* local_node = dynamic_cast<DeclarationNode*>(decl_node); 

    if(current_token->get_value() == "fn") 
    {
       bool flag = parse_fn_decl(current_token, local_node);

       if(flag)
       {

#if DEBUG_PARSER
    std::cout << local_node->decl_name << std::endl;
    std::cout << local_node->m_param_node->name << std::endl;
    std::cout << local_node->m_param_node->m_param_type->m_eReturnType << std::endl;
#endif
       }
    } 
    else if (current_token->get_value() == "let") 
    {
        // commented for now
        parse_var_decl(current_token, local_node);
    }

    return true;
}

bool Parser::parse_var_decl(Token* current_token, DeclarationNode* var_decl_node)  
{
    if(current_token->get_value() == ";") 
       return true; 
    else if (current_token->get_value() == "let") 
        var_decl_node->decl_name = peek()->get_value(); 
    else if (current_token->get_value() == ":") 
    {
       ReturnType var_type = TypeNode::valid_return_type(peek()->get_value());
       if (var_type == TYPE_INVALID) 
       {
          std::cerr << "Invalid type in code: " << peek()->get_value() << std::endl;
          // assert established for debugging otherwise it should be returning false in relases mode.
          assert(false); 
          return false; 
       }

       var_decl_node->m_type_node = new TypeNode(); 
       var_decl_node->m_type_node->m_eReturnType = var_type;
    } 
    else if (current_token->get_value() == "=") 
    {
    
    }

    parse_var_decl(get_next(), var_decl_node);

    return true;
}


// this function is not ending
bool Parser::parse_fn_decl(Token* current_token, DeclarationNode* decl_node) 
{
    if(current_token->get_value() == "fn") 
    {
       if (peek()->get_type() == IDENTIFIER) 
       {
           decl_node->decl_type = DeclarationType::FUNCTION;
           decl_node->decl_name = peek()->get_value();
           // send the parsing forward;
           parse_fn_decl(get_next(), decl_node);
       } 
       else 
          assert(!"Invalid syntax. Name of the function is supposed to follow after fn");
    }
    else if(current_token->get_value() == "(" && peek()->get_value() != ")") {
       if(!parse_fn_params(current_token, decl_node)) 
        {
            std::cerr << "Failed to parse fn parameters" << std::endl; 
            assert(false); 
            return false;
        }

        // this will be going foward in order to parse the block. 
        parse_fn_decl(get_next(), decl_node); 

    }
    // if current_token name is equal to decl_name that means the fn has been parsed 
    // and then we should move forward.
    else if (current_token->get_value() == decl_node->decl_name)
       parse_fn_decl(get_next(), decl_node);
     // this is parsing the function return type
    else if (current_token->get_value() == ":") 
    {
       if (!decl_node->m_type_node) 
           decl_node->m_type_node = new TypeNode();

       decl_node->m_type_node->m_eReturnType = TypeNode::valid_return_type(peek()->get_value());

        parse_fn_decl(skip_and_get(2) ,decl_node);
    } 

    else if (current_token->get_value() == "{") 
    {
        std::cout << "coming for block" << std::endl;
        StatementNode* statement = new StatementNode();
        assert(decl_node != nullptr);

        decl_node->m_stmts = statement;

        // parse_block will return true everytime a block has been parsed. 
        // this means when our stack top { comes across a new char which is } 
        // We want to return from the block then 

        block_level_updater(current_token);
        return parse_block(current_token, BLOCK_FUNC, statement);
    } 

    return true;
}


bool Parser::allocate_param_memory(Token* current_token) 
{
    // Allocate memory when we are getting ready to parse the new fn parameter
    if (current_token->get_value() == ",") 
       return true; 
    // allocate the memory when we are parsing the first paramater of the fn.
    else if (current_token->get_value() == "(") 
        return true;

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
        {

#if DEBUG_PARSER
            std::cout << "3" << std::endl;
#endif

           fn_decl_node->m_param_node = param;  
        }
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

#if DEBUG_PARSER
        std::cout << "Param parse completed" << std::endl; 
#endif
       return true; 
    } 
    else if (current_token->get_value() == "(") 
    {
       return parse_fn_params(get_next(), fn_decl_node);
    } 


    // Will be refactored for now we are working with a system that just checks if the value is a return type
    if (peek()->get_value() == ":") 
       latest_param->name = current_token->get_value();  
    else if (current_token->get_value() == ":") 
    {
       ReturnType param_type = TypeNode::valid_return_type(peek()->get_value());
       if (param_type == TYPE_INVALID) 
       {
          std::cerr << "Invalid type in code: " << peek()->get_value() << std::endl;
          // assert established for debugging otherwise it should be returning false in relases mode.
          assert(false); 
          return false; 
       }

       // new TypeNode memory being called here. 
       // Need to clean up the memory
       latest_param->m_param_type = new TypeNode(); 
       latest_param->m_param_type->m_eReturnType = param_type;
    }

    return parse_fn_params(get_next(), fn_decl_node);
}

// So this will be checking if the current token is a declation;
Parser::DeclType Parser::check_for_declartions(Token* current_token) 
{
    if (current_token->get_value() == "fn")
        return DECL_FN;

    if(current_token->get_value() == "let")
        return DECL_VAR;

    return DECL_NONE;
}

// if the current block level is greater 0 then we are in a statement block and the behaviour will be changing accordingly. 
bool Parser::get_current_block_level() 
{
    return m_sBlockStack.size();
}

Token* Parser::get_current() 
{
    if(m_Index >= m_vpInputTokens.size()) 
        return nullptr;

    return m_vpInputTokens[m_Index];
}

Token* Parser::peek() 
{
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
