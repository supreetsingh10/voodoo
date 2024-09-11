#pragma once
#include "./operators.hpp"
#include "datatypes.hpp"
#include <cstdint>
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

struct Node {
  Node() {}
  Node(NodeType e) : nodetype(e) {}
  virtual ~Node() {}

  NodeType nodetype;
  static void describe_tree(Node *n);
};

// Forward declaration
struct TypeNode;
struct ParamNode;
struct StatementNode;

struct DeclarationNode : public Node {
  DeclarationNode()
      : Node(DECLARATION), m_param_node(nullptr), m_stmts(nullptr),
        next(nullptr) {}
  virtual ~DeclarationNode();

  std::string decl_name;
  DeclarationType decl_type;
  // has the information about the return type.
  TypeNode *m_type_node;
  // Parameters. Can be none.
  ParamNode *m_param_node;
  // Points to the code inside a declaration.
  // Becomes useful when we are working with function declarations.
  StatementNode *m_stmts;
  // points to next declaration
  DeclarationNode *next;

  static void describe_decl(DeclarationNode *dn);
};

struct TypeNode;
// MAJOR REFACTOR
// Use this to create params for the functions.
// I can use this to always push to the end tail.
struct ParamNode : public Node {
  ParamNode() : m_param_type() {}
  virtual ~ParamNode();

  std::string name;
  TypeNode *m_param_type;
  // this will be pointing towards the next parameter.
  ParamNode *next;

  static void describe_param(ParamNode *p);
};

// Do not use this to create params for functions.
struct TypeNode : public Node {
  TypeNode() {}
  virtual ~TypeNode();

  DataType *m_data_node;
  TypeNode *m_subtype;
  static void describe_type(TypeNode *t);

  // MAJOR REFACTOR: Need to add move to a better check system for return type
  // validation static ReturnType valid_return_type(const std::string&
  // ret_type);
  static DataType *validate_token_type(const std::string &dat_type);
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
struct StatementNode : public Node {
  StatementNode()
      : m_stmt_decl(nullptr), m_ifbody(nullptr), m_else_body(nullptr),
        m_sub_block(nullptr), m_expr(nullptr), m_init_expr(nullptr),
        m_next_expr(nullptr), next(nullptr) {}

  virtual ~StatementNode();

  StatementType m_stmt_type;
  // Member objects
  DeclarationNode *m_stmt_decl;
  ExpressionNode *m_init_expr, *m_next_expr;
  ExpressionNode *m_expr;

  StatementNode *m_ifbody, *m_else_body;
  // In case there are sub blocks in the statement.
  StatementNode *m_sub_block;
  // this points to the next statement in the block.
  StatementNode *next;

  static void describe_stmt(StatementNode *s);
};

enum ExpressionOperator {
  // Mathematical expression
  // Binary expressions
  EXPR_ADD,
  EXPR_SUB,
  EXPR_MUL,
  EXPR_DIV,
  EXPR_MOD,

  // Binary
  EXPR_ASSIGN,
  // Boolean expressions
  // All are binary expression
  EXPR_EQUALS,
  EXPR_GREATER_THAN,
  EXPR_LESSER_THAN,
  EXPR_GREATER_EQUALS_TO,
  EXPR_LESSER_EQUALS_TO,
  EXPR_NOT_EQUALS,

  // Bitwise
  EXPR_AND,
  EXPR_NOT,
  EXPR_OR,
  EXPR_SEPARATOR,

  EXPR_SEMI_COLON,
  EXPR_COLON,
  EXPR_CALL,
  EXPR_ARG,
  EXPR_RETURN,
  EXPR_SUBSCRIPT,
  EXPR_NOPE,

  EXPR_CURL_BLOCK_BEGIN,
  EXPR_CURL_BLOCK_END,

  EXPR_RIGHT_PAREN,
  EXPR_LEFT_PAREN,

  EXPR_RIGHT_SQ,
  EXPR_LEFT_SQ,
};

enum ExprType {
  EXPR_OPERATOR,
  EXPR_OPERAND,
};

struct ExpressionNode : public Node {
  ExpressionNode()
      : Node(EXPRESSION), left(nullptr), right(nullptr), m_id_exp_op(-1)  {}
  virtual ~ExpressionNode();

  bool m_fn_call, m_op_brace;
  bool m_escalate_pres;
  ExpressionNode *left, *right;
  Operator *m_expr_opr;
  ExprType m_type;
  ExpressionOperator m_ExprOpr;

  std::string m_expr_name;
  DataType m_expr_data;
  int m_id_exp_op;
  const int get_expr_precedence() const;

  static int last_added_op_node_id;
  static ExpressionNode *last_added_op_node;

  static ExpressionNode *get_expression_node_with_id(ExpressionNode *node_root,
                                                     const int32_t node_id,
                                                     bool parent);
  static ExpressionNode *
  get_suitable_parent_node(ExpressionNode *node_root,
                           const ExpressionNode *child_node);
};
