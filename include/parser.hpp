#pragma once

#include <stack>
#include <vector>
#include "./token.hpp"
#include "./ast.hpp"


class Parser {
    public: 
        Parser(): m_ast_root(nullptr), m_trav_node(nullptr), m_Index((0)) {} 
        ~Parser() {} 

        Token* get_next(); 
        Token* peek();
        Token* get_current(); 

        void set_input_stream(const std::vector<Token*>& input_stream); 

        bool parse(); 

        enum DeclType {
           DECL_FN,
           DECL_VAR,
           DECL_NONE,
           DECL_ARR,
        };

        // TODO: Implement
        // Returns the type of declation we will be parsing. 
        DeclType check_for_declartions(Token* current_token); 
        // this returns the level of blocks we are currently in. 
        // This will be useful in making statement code. 
        bool get_current_block_level();

    private: 
        bool parse(Token* current_token); 
        bool parse_decl(Token* current_token, Node* decl_node); 
        bool parse_block(Token* current_token, BlockType block_type, StatementNode* block_node); 

        bool parse_var_decl(Token* current_token, DeclarationNode* var_decl_node); 
        bool block_parse_completed(Token* current_token);

        bool parse_fn_decl(Token* current_token, DeclarationNode* fn_decl_node); 
        bool parse_fn_params(Token* current_token, DeclarationNode* fn_decl_node); 
        bool allocate_param_memory(Token* current_param_token); 
        int get_fn_param_count(); 

        Node* m_ast_root, *m_trav_node, *m_parsed_node;
        int m_Index; 
        std::vector<Token*> m_vpInputTokens; 

        std::stack<int> m_sBlockCounter;
};
