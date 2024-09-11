#include "../include/parser.hpp"
#include "../include/datatypes.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#define DEBUG_PARSER false

void Parser::set_input_stream(const std::vector<Token *> &input_stream) {
  m_vpInputTokens = input_stream;
}

bool Parser::parse() {
  while (m_Index < m_vpInputTokens.size()) {
    bool success_parse = parse(get_current());

    if (!success_parse) {
      assert("Failed to parse");
      return false;
    } else
      m_Index++;
  }

  return true;
}

bool Parser::parse(Token *current_token) {
  int block_levels = get_current_block_level();

  // if we are not in a block
  if (block_levels == 0) {
    Parser::DeclType decl_type = check_for_declartions(current_token);

    if (decl_type != DECL_NONE) {
      DeclarationNode *decl_node = new DeclarationNode();
      // this is required for dynamic casting.
      // In the coming time we will have to dynamic cast the nodes to required
      // child classes. This is where we will know what child class to cast
      decl_node->nodetype = DECLARATION;
      parse_decl(current_token, decl_node);

      if (!m_bStartNodeSet) {
        m_StartNode = decl_node;
        m_bStartNodeSet = true;
      } else {
        DeclarationNode *temp = dynamic_cast<DeclarationNode *>(m_StartNode);
        // Always next to the latest declaration.
        while (temp->next != nullptr)
          temp = temp->next;

        temp->next = decl_node;
      }
    }
  }

#if DEBUG_PARSER
  Node::describe_tree(m_StartNode);
#endif

  return true;
}

Token *Parser::get_nth_from_current(const size_t &skip_num) {
  assert(skip_num + m_Index < m_vpInputTokens.size());
  m_Index += skip_num;

  return m_vpInputTokens[m_Index];
}

// if we are on the end of the statement then we are doing this.
bool Parser::allocate_stmt_memory(Token *current_stmt_token) {
  if (current_stmt_token->get_value() == ";")
    return true;
  else if (current_stmt_token->get_value() == "{")
    return true;

  return false;
}

// parsing of block should end when the last "}" is reached for a block.
bool Parser::parse_block(Token *current_token, BlockType block_type,
                         StatementNode *block_node) {
  if (current_token->get_value() == "}") {
    block_level_updater(current_token);
    return true;
  }

  StatementNode *temp_node = nullptr;

  if (allocate_stmt_memory(current_token)) {
    // this will chain our statement blocks.
    temp_node = new StatementNode();
    block_node->next = temp_node;
  } else
    temp_node = block_node;

  assert(temp_node != nullptr && "The temp node is null");

  DeclType local_decl_type = check_for_declartions(current_token);
  if (local_decl_type != DECL_NONE) {
    if (local_decl_type == DECL_FN) {
      std::cerr << "Cannot declare functions inside a block" << std::endl;
      assert(false);
      return false;
    } else if (local_decl_type == DECL_VAR) {
      DeclarationNode *local_decl_node = new DeclarationNode();

      if (temp_node == nullptr) {
        std::cerr << "Statement node happens to be null" << std::endl;
        assert(false);
        return false;
      }

      if (temp_node->m_stmt_decl == nullptr)
        temp_node->m_stmt_decl = local_decl_node;
      else {
        DeclarationNode *latest_decl = temp_node->m_stmt_decl;

        assert(latest_decl != nullptr && "Declaration is null here");

        while (latest_decl->next != nullptr)
          latest_decl = latest_decl->next;

        latest_decl->next = local_decl_node;
      }

      if (!parse_var_decl(current_token, local_decl_node)) {
        std::cerr << "Failed to parse the local variable "
                  << current_token->get_value() << std::endl;
        assert(false);
        return false;
      }
    }
  } else if (current_token->get_value() == "{") {
    assert(temp_node != nullptr);
    // always get the latest node in the code block.
    StatementNode *local_code_block = temp_node->m_sub_block;

    if (temp_node->m_sub_block == nullptr) {
      temp_node->m_sub_block = new StatementNode();
      local_code_block = temp_node->m_sub_block;
    } else {
      // In case we have multiple code blocks in a fn.
      // We always want to go to the last one and add a new block to it.
      while (local_code_block->next != nullptr)
        local_code_block = local_code_block->next;

      local_code_block->next = new StatementNode();
      local_code_block = local_code_block->next;
    }

    // We are sending in the next token to parse
    // We do not need to handle the { brace here
    block_level_updater(current_token);
    local_code_block->m_stmt_type = STMT_BLOCK;
    assert(local_code_block != nullptr && "Block code is null");

    parse_block(get_next(), BLOCK_NORMAL, local_code_block);
  } else if (current_token->get_value() == "if") {
    assert(block_node->m_ifbody == nullptr);

    block_node->m_ifbody = new StatementNode();
    parse_if_expr(current_token, block_node->m_ifbody);
  } else if (current_token->get_type() == IDENTIFIER) {
    ExpressionNode *out = nullptr;
    assert(block_node->m_expr == nullptr);
    block_node->m_expr = new ExpressionNode();
    parse_expr(current_token, block_node->m_expr, out);
  }
  // TODO Need to add the else block here,
  // These blocks will have if, if else, and else statements.

  parse_block(get_next(), block_type, block_node);
  return true;
}

// There should be a latest operator node. Which has the last added operator
// cached. The root of the expression will be the expression which will be of
// lowest presedence. i.e. which will have highest presedence value. This
// function will only be responsible for placing the node in the this is not
// taking
bool Parser::update_root(ExpressionNode *node, ExpressionNode *expr_root) {
  if (!expr_root) {
    expr_root = node;
  } else {
    if (expr_root->m_type == EXPR_OPERAND) {
      if (node->m_type == EXPR_OPERATOR) {
        ExpressionNode *temp = expr_root;
        expr_root = node;
        expr_root->left = temp;
        // Static value.
        // Incremented when the Operator is inserted into the
        ExpressionNode::last_added_op_node_id += 1;
        ExpressionNode::last_added_op_node = node;
      } else {
        std::cout << "Weird expression going on here." << std::endl;
        assert(false);
        return false;
      }
    } else {
      // expression_root is operator here
      if (node->m_type == EXPR_OPERAND) {
        if (!ExpressionNode::last_added_op_node->left)
          ExpressionNode::last_added_op_node->left = node;
        else if (!ExpressionNode::last_added_op_node->right)
          ExpressionNode::last_added_op_node->right = node;
        else {
          std::cerr
              << "No Operator node seems to be available for this operand "
              << node->m_expr_name << std::endl;
          assert(false);
          return false;
        }
      } else if (node->m_type == EXPR_OPERATOR) {
        ExpressionNode *parent_node =
            ExpressionNode::get_suitable_parent_node(expr_root, node);
        ExpressionNode::last_added_op_node_id += 1;
        ExpressionNode::last_added_op_node = node;

        if (parent_node->left == nullptr)
          parent_node->left = node;
        else if (parent_node->right == nullptr)
          parent_node->right = node;
        else if (parent_node->right != nullptr) {
          ExpressionNode *local_node = parent_node->right;
          parent_node->right = node;
          node->left = local_node;
        } else {
          std::cerr << "Failed to place the operator node" << std::endl;
          assert(false && "Failed to place the operator node");
          return false;
        }
      }
    }
  }
  return true;
}

bool Parser::parse_expr(Token *current_token, ExpressionNode *expr_stmt,
                        ExpressionNode *expr_root) {
  // Marks the end of the expression parse.
  if (current_token->get_type() == IDENTIFIER) {
    expr_stmt->m_type = EXPR_OPERAND;
    expr_stmt->m_expr_name = current_token->get_value();
    if (peek()->get_value() == "(" && peek()->get_type() == OPERATOR)
      expr_stmt->m_fn_call = true;
  } else if (current_token->get_type() == NUMBER) {
    expr_stmt->m_type = EXPR_OPERAND;
    expr_stmt->m_expr_data.m_data_value = current_token->get_value();
    expr_stmt->m_expr_data.m_data_type =
        DataType::number_type_from_literal(current_token->get_value());

    if (peek()->get_value() == "(" && peek()->get_type() == OPERATOR)
      expr_stmt->m_op_brace = true;

  } else if (current_token->get_type() == OPERATOR) {
    Operators *op = dynamic_cast<Operators *>(current_token);
    assert(op != nullptr && "Failed to dynamic cast the operator.");

    expr_stmt->m_type = EXPR_OPERATOR;
    expr_stmt->m_id_exp_op = ExpressionNode::last_added_op_node_id;

    switch (op->m_operator.get_operator_enum()) {
    case NOPE:
      std::cerr << "It is going in Nope " << op->get_value() << std::endl;
      break;
    case ADDITION:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_ADD;
      break;
    case SUBTRACTION:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_SUB;
      break;
    case DIVISION:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_DIV;
      break;
    case MULTIPLICATION:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case PERCENT:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MOD;
      break;
    case GREATER_THAN:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_GREATER_THAN;
      break;
    case LESSER_THAN:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_LESSER_THAN;
      break;
    case ASSIGNMENT:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_ASSIGN;
      break;
    case EQUALS:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_EQUALS;
      break;
    case NOT_EQUAL:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_NOT_EQUALS;
      break;
    case LESS_THAN_EQUAL:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_LESSER_EQUALS_TO;
      break;
    case GREATER_THAN_EQUAL:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_GREATER_EQUALS_TO;
      break;
    case RIGHT_PAREN:
      // returns the node with the and the expression token. The root is fresh
      return true;
    case LEFT_PAREN:
      // TODO: need to check for valid braces
      if (expr_stmt->m_fn_call) {
        // parse for function call.
      } else {
        expr_stmt->m_op_brace = true;
      }
      break;
    case RIGHT_CURLY:
      // we can skip this one.
      // Equate this to nope.
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_CURL_BLOCK_BEGIN;
      break;
    case LEFT_CURLY:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_CURL_BLOCK_END;
      break;
    case RIGHT_SQUARE_BRACKET:
      // Indexing operator.
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_RIGHT_SQ;
      break;
    case LEFT_SQUARE_BRACKET:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_LEFT_SQ;
      break;
    case UNDERSCORE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case DASH:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case COLON:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case SEMI_COLON:
      // we can return this and use it to make as the EOL expression.
      // marks the end of the expression.
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_SEMI_COLON;
      return true;
    case AT_RATE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case DOLLAR:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case QUESTION_MARK:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case STRAIGHT_BAR:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case TILDE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case BACK_TICK:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case HASH:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_NOPE;
      break;
    case EXCLAIMATION_MARK:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_NOT;
      break;
    case CARET:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case COMMA:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_SEPARATOR;
      break;
    case AMPERSAND:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case BACK_SLASH:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case DOT:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case DOUBLE_QUOTE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case RIGHT_DOUBLE_QUOTE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case LEFT_DOUBLE_QUOTE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case RIGHT_SINGLE_QUOTE:
      expr_stmt->m_ExprOpr = ExpressionOperator::EXPR_MUL;
      break;
    case LEFT_SINGLE_QUOTE:
      break;
    };
  }

  ExpressionNode *brace_root = nullptr;
  if (expr_stmt->m_op_brace) {
    ExpressionNode *brace_stmt = new ExpressionNode();
    if (parse_expr(get_next(), brace_stmt, brace_root)) {
      brace_root->m_escalate_pres = true;
      update_root(expr_stmt, brace_root);
    }
  }

  update_root(expr_stmt, expr_root);
  ExpressionNode *next_expr = new ExpressionNode();
  parse_expr(get_next(), next_expr, expr_root);

  return true;
}

std::vector<Token *> Parser::get_line_tokens() const {
  size_t current = m_Index;

  while (m_vpInputTokens[current]->get_value() != ";" && current > 0) {
    current--;
  }

  ++current;
  std::vector<Token *> line_tokens;

  while (m_vpInputTokens[current]->get_value() != ";" &&
         current < m_vpInputTokens.size()) {
    line_tokens.push_back(m_vpInputTokens[current]);
    ++current;
  }

  return line_tokens;
}

bool Parser::parse_if_expr(Token *current_token, StatementNode *expr_stmt) {

  return true;
}

bool Parser::block_parse_completed(Token *current_token) {
  if (current_token->get_value() == "}") {
    if (m_sBlockStack.size() == 0) {
      std::cerr << "Improper usage of scope operator" << std::endl;
      assert(false);
      return false;
    }

    m_sBlockStack.pop();
  }

  return true;
}

void Parser::block_level_updater(Token *current_token) {
  if (current_token->get_value() == "{")
    m_sBlockStack.push('{');
  else if (current_token->get_value() == "}") {
    if (m_sBlockStack.size() == 0) {
      std::cerr << "Improper usage of }" << std::endl;
      assert(false);
    }

    m_sBlockStack.pop();
  }

#if DEBUG_PARSER
  std::cout << "Block level " << m_sBlockStack.size() << std::endl;
#endif
}

// this will be creating a declation object which will be storing the required
// values. we will typecast
bool Parser::parse_decl(Token *current_token, Node *decl_node) {
  DeclarationNode *local_node = dynamic_cast<DeclarationNode *>(decl_node);

  if (current_token->get_value() == "fn") {
    bool flag = parse_fn(current_token, local_node);
    assert(flag && "Failed to parse the fn declaration");
    return flag;
  } else if (current_token->get_value() == "let")
    parse_var_decl(current_token, local_node);

  return true;
}

bool Parser::parse_var_decl(Token *current_token,
                            DeclarationNode *var_decl_node) {
  if (current_token->get_value() == ";")
    return true;
  else if (current_token->get_value() == "let")
    var_decl_node->decl_name = peek()->get_value();
  else if (current_token->get_value() == ":") {
    DataType *var_type = TypeNode::validate_token_type(peek()->get_value());
    if (var_type->m_data_type == TYPE_INVALID) {
      std::cerr << "Invalid type in code: " << peek()->get_value() << std::endl;
      // assert established for debugging otherwise it should be returning false
      // in relases mode.
      assert(false);
      return false;
    }

    var_decl_node->m_type_node = new TypeNode();
    var_decl_node->m_type_node->m_data_node = var_type;
  } else if (current_token->get_value() == "=") {
    if (var_decl_node->m_type_node == nullptr) {
      std::cerr << "Type node found null" << std::endl;
      assert(false);
      return false;
    }

    var_decl_node->m_type_node->m_data_node->m_data_value =
        get_next()->get_value();
  }

  parse_var_decl(get_next(), var_decl_node);

  return true;
}

// this function is not ending
bool Parser::parse_fn(Token *current_token, DeclarationNode *decl_node) {
  if (current_token->get_value() == "fn") {
    if (peek()->get_type() == IDENTIFIER) {
      decl_node->decl_type = DeclarationType::FUNCTION;
      decl_node->decl_name = peek()->get_value();
      // send the parsing forward;
      parse_fn(get_next(), decl_node);
    } else
      assert(!"Invalid syntax. Name of the function is supposed to follow "
              "after fn");
  }
  // if there are parameters we are parsing the params.
  else if (current_token->get_value() == "(" && peek()->get_value() != ")") {
    if (!parse_fn_params(current_token, decl_node)) {
      std::cerr << "Failed to parse fn parameters" << std::endl;
      assert(false);
      return false;
    }

    // this will be going foward in order to parse the block.
    parse_fn(get_next(), decl_node);
  } else if (current_token->get_value() == "(" && peek()->get_value() == ")")
    parse_fn(get_nth_from_current(static_cast<size_t>(2)), decl_node);
  // This has been added in order to make sure that we do not stay in the
  // parsing loop when
  else if (current_token->get_value() == ")")
    parse_fn(get_next(), decl_node);
  else if (current_token->get_value() == decl_node->decl_name)
    parse_fn(get_next(), decl_node);
  else if (current_token->get_value() == ":") {
    if (!decl_node->m_type_node)
      decl_node->m_type_node = new TypeNode();

    decl_node->m_type_node->m_data_node =
        TypeNode::validate_token_type(peek()->get_value());

    parse_fn(get_nth_from_current(static_cast<size_t>(2)), decl_node);
  } else if (current_token->get_value() == "{") {
    StatementNode *out_statement = new StatementNode();
    assert(decl_node != nullptr && "Decl node cannot be a nullptr");

    // parse_block will return true everytime a block has been parsed.
    // this means when our stack top { comes across a new char which is }
    // We want to return from the block then

    block_level_updater(current_token);
    bool flag = parse_block(get_next(), BLOCK_FUNC, out_statement);

    assert(flag && "Failed to parse block");
    assert(out_statement != nullptr && "Out statement is nullptr");

    decl_node->m_stmts = out_statement;
    return flag;
  }

  return true;
}

bool Parser::allocate_param_memory(Token *current_token) {
  if (current_token->get_value() == ",")
    return true;
  else if (current_token->get_value() == "(")
    return true;

  return false;
}

// will not be called when there are no parameters.
bool Parser::parse_fn_params(Token *current_token,
                             DeclarationNode *fn_decl_node) {
  ParamNode *latest_param = nullptr, *temp = nullptr;

  if (allocate_param_memory(current_token)) {
    ParamNode *param = nullptr;
    param = new ParamNode();

    if (fn_decl_node->m_param_node == nullptr)
      fn_decl_node->m_param_node = param;
    else {
      temp = fn_decl_node->m_param_node;

      while (temp->next != nullptr)
        temp = temp->next;

      temp->next = param;
      // points to the last parameter;
      latest_param = temp->next;
    }
  }
  // if we are not allocating memory I still want to be on the last param node.
  else {
    temp = fn_decl_node->m_param_node;

    while (temp->next != nullptr)
      temp = temp->next;

    latest_param = temp;

    assert(latest_param != nullptr);
  }

  if (current_token->get_value() == ")")
    return true;
  else if (current_token->get_value() == "(")
    return parse_fn_params(get_next(), fn_decl_node);

  // Will be refactored for now we are working with a system that just checks if
  // the value is a return type
  if (peek()->get_value() == ":")
    latest_param->name = current_token->get_value();
  else if (current_token->get_value() == ":") {
    DataType *param_type = TypeNode::validate_token_type(peek()->get_value());
    if (param_type->m_data_type == TYPE_INVALID) {
      assert(("Invalid type in code " && false));
      return false;
    }

    latest_param->m_param_type = new TypeNode();
    latest_param->m_param_type->m_data_node = param_type;
  }

  return parse_fn_params(get_next(), fn_decl_node);
}

// So this will be checking if the current token is a declation;
Parser::DeclType Parser::check_for_declartions(Token *current_token) {
  if (current_token->get_value() == "fn")
    return DECL_FN;

  if (current_token->get_value() == "let")
    return DECL_VAR;

  return DECL_NONE;
}

// if the current block level is greater 0 then we are in a statement block and
// the behaviour will be changing accordingly.
size_t Parser::get_current_block_level() const { return m_sBlockStack.size(); }

Token *Parser::get_current() const {
  if (m_Index >= m_vpInputTokens.size())
    return nullptr;

  return m_vpInputTokens[m_Index];
}

Token *Parser::peek() {
  if (m_Index >= m_vpInputTokens.size())
    return nullptr;

  return m_vpInputTokens[m_Index + 1];
}

// moves the current location of the token.
Token *Parser::get_next() {
  // increment the index and check for the next one
  ++m_Index;
  if (m_Index >= m_vpInputTokens.size()) {
    return nullptr;
  }

  return m_vpInputTokens[m_Index];
}
