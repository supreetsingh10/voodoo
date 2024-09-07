#include "../include/ast.hpp"
#include "../include/datatypes.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <utility>

void Node::describe_tree(Node *n) {

  if (!n)
    return;

  if (n->nodetype == DECLARATION)
    DeclarationNode::describe_decl(dynamic_cast<DeclarationNode *>(n));
  else if (n->nodetype == STATEMENT)
    StatementNode::describe_stmt(dynamic_cast<StatementNode *>(n));
}

void DeclarationNode::describe_decl(DeclarationNode *dn) {
  if (!dn)
    return;

  std::cout << "Declaration name " << dn->decl_name << std::endl;
  std::cout << "Declaration " << dn->decl_type << std::endl;

  TypeNode::describe_type(dn->m_type_node);
  ParamNode::describe_param(dn->m_param_node);
  StatementNode::describe_stmt(dn->m_stmts);

  describe_decl(dn->next);
}

DeclarationNode::~DeclarationNode() {
  if (m_type_node)
    delete m_type_node;

  if (m_stmts)
    delete m_stmts;

  if (m_param_node)
    delete m_param_node;
}

ParamNode::~ParamNode() {
  if (m_param_type)
    delete m_param_type;
}

TypeNode::~TypeNode() {
  if (m_subtype)
    delete m_subtype;

  if (m_data_node)
    delete m_data_node;
}

void TypeNode::describe_type(TypeNode *t) {
  if (!t)
    return;

  std::cout << "Type node value " << t->m_data_node->m_data_type << " "
            << t->m_data_node->m_data_value << std::endl;
}

void ParamNode::describe_param(ParamNode *p) {
  if (!p)
    return;

  std::cout << "Param node " << p->name << std::endl;
  describe_param(p->next);
}

StatementNode::~StatementNode() {
  if (m_stmt_decl)
    delete m_stmt_decl;
}

void StatementNode::describe_stmt(StatementNode *s) {
  if (!s)
    return;

  DeclarationNode::describe_decl(s->m_stmt_decl);

  std::cout << "This is a statement node " << std::endl;
  describe_stmt(s->next);

  std::cout << "Code block below " << std::endl;
  describe_stmt(s->m_sub_block);
}

DataType *TypeNode::validate_token_type(const std::string &dat_type) {
  // it can go through the parse tree to check if the declaration is user
  // defined. this is not optimal

  std::map<std::string, DATA_TYPE> m_RetTypes;
  m_RetTypes.insert(std::make_pair("void", TYPE_VOID));
  m_RetTypes.insert(std::make_pair("str", TYPE_STRING));
  m_RetTypes.insert(std::make_pair("char", TYPE_CHAR));
  m_RetTypes.insert(std::make_pair("int", TYPE_INT));
  m_RetTypes.insert(std::make_pair("deci", TYPE_DECI));

  auto val = m_RetTypes.find(dat_type);

  if (val == m_RetTypes.end()) {
    return nullptr;
  }

  DataType *d = new DataType(val->second);
  return d;
}

ExpressionNode::~ExpressionNode() {
  delete left;
  delete right;
}

int ExpressionNode::last_added_op_node_id = 0;
ExpressionNode *ExpressionNode::last_added_op_node = nullptr;

ExpressionNode *ExpressionNode::get_expression_node_with_id(
    ExpressionNode *expr_root, const int32_t node_id, bool parent_flag) {
  if (expr_root == nullptr)
    return nullptr;

  if (parent_flag) {
    if (!expr_root->left && expr_root->left->m_id_exp_op == node_id)
      return expr_root->left;
    else if (!expr_root->right && expr_root->right->m_id_exp_op == node_id)
      return expr_root->right;
  } else {
    if (expr_root->m_id_exp_op == node_id || expr_root->m_id_exp_op == node_id)
      return expr_root;
  }

  ExpressionNode *local_node = nullptr;
  local_node =
      get_expression_node_with_id(expr_root->left, node_id, parent_flag);
  if (local_node != nullptr)
    return local_node;

  local_node =
      get_expression_node_with_id(expr_root->right, node_id, parent_flag);
  if (local_node != nullptr)
    return local_node;

  // in case there is no node
  return local_node;
}

const int ExpressionNode::get_expr_precedence() const {
  assert(this->m_expr_opr);
  return this->m_expr_opr->get_precedence();
}

ExpressionNode *
ExpressionNode::get_suitable_parent_node(ExpressionNode *node_root,
                                         const ExpressionNode *child_node) {
  if (ExpressionNode::last_added_op_node_id == 1)
    return last_added_op_node;

  ExpressionNode *last_node_parent =
      get_expression_node_with_id(node_root, last_added_op_node_id, true);

  ExpressionNode *last_node = nullptr;
  if (last_node_parent->left && last_node_parent->left->m_id_exp_op ==
                                    ExpressionNode::last_added_op_node_id)
    last_node = last_node_parent->left;
  else if (last_node_parent->right && last_node_parent->right->m_id_exp_op ==
                                          ExpressionNode::last_added_op_node_id)
    last_node = last_node_parent->right;

  return (last_node->get_expr_precedence() >= child_node->get_expr_precedence())
             ? last_node
             : last_node_parent;
}
