#pragma once

#include <stack>
#include <sys/types.h>
#include <vector>
#include "./token.hpp"
#include "./ast.hpp"


class Parser {
    public:
        Parser(): m_StartNode(nullptr), m_Index((0)), m_bStartNodeSet(false) {} 
        ~Parser() {} 

        Token* get_next(); 
        Token* get_nth_from_current(const size_t& skip_num); 
        Token* peek();
        Token* get_current() const; 

        void set_input_stream(const std::vector<Token*>& input_stream); 

        bool parse(); 

        enum DeclType {
           DECL_FN,
           DECL_VAR,
           DECL_NONE,
           DECL_ARR,
        };

        // TODO
        // implement the return types for the blocks.
        // They will be using our dataype code.

        // TODO: Implement
        // Returns the type of declation we will be parsing. 
        DeclType check_for_declartions(Token* current_token); 
        int check_for_expression(Token* current_token) const;
        // this returns the level of blocks we are currently in. 
        // This will be useful in making statement code. 
        size_t get_current_block_level() const;

    protected:
        // master functions.
        bool update_root(ExpressionNode* node, ExpressionNode* expr_root);
        bool parse(Token* current_token); 
        bool parse_decl(Token* current_token, Node* decl_node); 
        bool parse_block(Token* current_token, BlockType block_type, StatementNode* block_node); 
        bool parse_if_expr(Token* current_token, StatementNode* expr_stmt);
        bool parse_expr(Token* current_token, ExpressionNode* expr_node, ExpressionNode* expr_root);

        bool parse_var_decl(Token* current_token, DeclarationNode* var_decl_node); 
        bool block_parse_completed(Token* current_token);

        bool parse_fn(Token* current_token, DeclarationNode* fn_decl_node); 
        bool parse_fn_params(Token* current_token, DeclarationNode* fn_decl_node); 

        bool allocate_param_memory(Token* current_param_token); 

        int get_fn_param_count();
        std::vector<Token*> get_line_tokens() const; 


        Node* m_StartNode; 
        bool m_bStartNodeSet; 
        size_t m_Index; 


    private:
        // statement parsing functions.
        bool allocate_stmt_memory(Token* current_stmt_token);
        void block_level_updater(Token* current_token); 

        std::vector<Token*> m_vpInputTokens; 
        std::stack<char> m_sBlockStack;
};
