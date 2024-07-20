#pragma once
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

struct Node {
    Node() {}
    Node(NodeType e): nodetype(e) {}
    virtual ~Node() {}

    NodeType nodetype;
};

// Forward declaration 
struct TypeNode;
struct ParamNode;


struct DeclarationNode: public Node {
    DeclarationNode(): Node(DECLARATION) {}
    virtual ~DeclarationNode() {}

    // has the information about the return type.
    TypeNode* m_type;
    // Parameters. Can be none. 
    ParamNode* m_param_type;
    // points to next declaration 
    DeclarationNode *next; 
};

enum ReturnType {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
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

    ReturnType m_eReturnType;
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
        m_else_body(else_body),
        Node(STATEMENT)
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
    ExpressionNode(): Node(EXPRESSION) {}
    virtual ~ExpressionNode() {}
};
