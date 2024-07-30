#include "../include/ast.hpp"
#include "../include/datatypes.hpp"
#include <map>
#include <utility>
#include <iostream>


void Node::describe_tree(Node *n) 
{
     if (n->nodetype == DECLARATION) 
         DeclarationNode::describe_decl(dynamic_cast<DeclarationNode*>(n));
}


void DeclarationNode::describe_decl(DeclarationNode *dn) 
{
      if (!dn)
         return;

      std::cout <<"Declaration name " << dn->decl_name << std::endl;
      std::cout <<"Declaration " << dn->decl_type << std::endl;

      describe_decl(dn->next);

      TypeNode::describe_type(dn->m_type_node);
      StatementNode::describe_stmt(dn->m_stmts);
}

DeclarationNode::~DeclarationNode() 
{
   if(m_type_node)
      delete m_type_node;

   if(m_stmts)
      delete m_stmts;

   if(m_param_node)
      delete m_param_node;
}

ParamNode::~ParamNode() 
{
   if(m_param_type)
      delete m_param_type;
}

TypeNode::~TypeNode() 
{
   if(m_subtype)
      delete m_subtype;

   if(m_data_node)
        delete m_data_node;
}

void TypeNode::describe_type(TypeNode *t) 
{
      if(!t)
         return;

      std::cout << "Type node value " << t->m_data_node->m_data_type << " " << t->m_data_node->m_data_value << std::endl;
}

StatementNode::~StatementNode() 
{
   if(m_stmt_decl)
      delete m_stmt_decl;

}

void StatementNode::describe_stmt(StatementNode *s) 
{
      if(!s)
         return;


      DeclarationNode::describe_tree(s->m_stmt_decl);
      describe_stmt(s->next);
}

DataType* TypeNode::validate_token_type(const std::string &dat_type) 
{
   // it can go through the parse tree to check if the declaration is user defined. 
   // this is not optimal

   std::map<std::string, DATA_TYPE> m_RetTypes;
   m_RetTypes.insert(std::make_pair("void", TYPE_VOID));
   m_RetTypes.insert(std::make_pair("str", TYPE_STRING));
   m_RetTypes.insert(std::make_pair("char", TYPE_CHAR));
   m_RetTypes.insert(std::make_pair("int", TYPE_INT));
   m_RetTypes.insert(std::make_pair("deci", TYPE_DECI));

    auto val = m_RetTypes.find(dat_type);

    if (val == m_RetTypes.end()) 
   {
      return nullptr;
   }

   DataType *d = new DataType(val->second);
   return d;
}



