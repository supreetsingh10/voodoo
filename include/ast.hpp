#pragma once
#include "datatypes.hpp"
#include <string>

enum NodeType {
    DECLARATION,
    STATEMENT,
    EXPRESSION,
};

enum DeclarationType {
    VARIABLE,
    FUNCTION,
};

enum BlockType {
    BLOCK_FOR_LOOP,
    BLOCK_FUNC,
    BLOCK_IF,
    BLOCK_ELSE,
    BLOCK_NORMAL,
}; 

struct Node 
{
    Node() {}
    Node(NodeType e): nodetype(e) {}
    virtual ~Node() {}

    NodeType nodetype;
    static void describe_tree(Node* n); 
};

// Forward declaration 
struct TypeNode;
struct ParamNode;
struct StatementNode; 

struct DeclarationNode: public Node 
{
    DeclarationNode(): Node(DECLARATION), m_param_node(nullptr), m_stmts(nullptr), next(nullptr) {}
    virtual ~DeclarationNode();


    std::string decl_name;
    DeclarationType decl_type;
    // has the information about the return type.
    TypeNode* m_type_node;
    // Parameters. Can be none. 
    ParamNode* m_param_node;
    // Points to the code inside a declaration. 
    // Becomes useful when we are working with function declarations.
    StatementNode* m_stmts;
    // points to next declaration 
    DeclarationNode *next; 

    static void describe_decl(DeclarationNode* dn);
};

struct TypeNode; 
// MAJOR REFACTOR
// Use this to create params for the functions.
// I can use this to always push to the end tail. 
struct ParamNode: public Node 
{
    ParamNode(): m_param_type() {}
    virtual ~ParamNode();

    std::string name;
    TypeNode* m_param_type;
    // this will be pointing towards the next parameter. 
    ParamNode* next;
}; 

// Do not use this to create params for functions.
struct TypeNode: public Node 
{
    TypeNode() {}
    virtual ~TypeNode();

    DataType* m_data_node;
    TypeNode* m_subtype;
    static void describe_type(TypeNode* t); 

    // MAJOR REFACTOR: Need to add move to a better check system for return type validation
    // static ReturnType valid_return_type(const std::string& ret_type); 
    static DataType* validate_token_type(const std::string& dat_type); 

};

enum StatementType {
    STMT_DECL,
    STMT_EXPR,
    STMT_IF,
    STMT_ELSE_IF,
    STMT_ELSE,
    STMT_IF_ELSE,
    STMT_FOR,
    STMT_RETURN,
    STMT_BLOCK,
};

struct ExpressionNode; 
struct StatementNode: public Node {
    // Constuctors
    StatementNode():
        m_stmt_decl(nullptr),
        m_ifbody(nullptr),
        m_else_body(nullptr),
        m_code_block(nullptr),
        next(nullptr)
    {}
    StatementNode(
            DeclarationNode* decl_node,
            ExpressionNode* init_expr,
            ExpressionNode* expr,
            ExpressionNode* next_expr,
            StatementNode* body, 
            StatementNode* else_body
            ): 
        m_stmt_decl(decl_node),
        m_init_expr(init_expr),
        m_expr(expr),
        m_next_expr(next_expr),
        m_ifbody(body),
        m_else_body(else_body),
        Node(STATEMENT)
    {}

    virtual ~StatementNode();

    StatementType m_stmt_type;
    // Member objects
    DeclarationNode* m_stmt_decl;
    ExpressionNode* m_init_expr, *m_expr, *m_next_expr;
    StatementNode* m_ifbody, *m_else_body; 
    // In case there are sub blocks in the statement.
    StatementNode* m_code_block;

    // this points to the next statement in the block.
    StatementNode* next;

    static void describe_stmt(StatementNode* s);
};


enum ExpressionType {
    EXPR_ASSIGN,
    EXPR_ADD,
    EXPR_COLON,
};


// TODO to set up expressions.
struct ExpressionNode: public Node {
    ExpressionNode(): Node(EXPRESSION) {}
    virtual ~ExpressionNode() {}
};
