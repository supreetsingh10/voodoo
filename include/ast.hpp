#pragma once
#include <string>

struct Node {
    Node() {}
    ~Node() {}
};

// Forward declaration 
struct TypeNode;

struct DeclarationNode : public Node {
    DeclarationNode() {}
    virtual ~DeclarationNode() {}

    TypeNode* m_type;
    DeclarationNode *next; 
};

enum Types {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FUNCTION,
};

struct TypeNode; 
struct ParamNode: public Node {
    ParamNode() {}
    virtual ~ParamNode() {}

    std::string name;
    TypeNode* m_param_type;
    ParamNode* next;
}; 

struct TypeNode: public Node {
    TypeNode() {}
    virtual ~TypeNode() {}

    Types m_etype;
    TypeNode* m_subtype;
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
    StatementNode() {}
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
        m_body(body),
        m_else_body(else_body)
    {}

    virtual ~StatementNode() {}

    StatementType m_stmt_type;
    // Member objects
    DeclarationNode* m_stmt_decl;
    ExpressionNode* m_init_expr, *m_expr, *m_next_expr;
    StatementNode* m_body, *m_else_body;
    StatementNode* next;
};


enum ExpressionType {
    EXPR_ASSIGN,
    EXPR_ADD,
    EXPR_COLON,
};


// TODO to set up expressions.
struct ExpressionNode: public Node {
    ExpressionNode() {}
    virtual ~ExpressionNode() {}
};
