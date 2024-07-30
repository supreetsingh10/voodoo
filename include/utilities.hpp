#pragma once

#define SAFE_RELEASE_VECTOR(vec) for(auto e : vec) delete e
#define POPULATE_MAP(m, start, end, type) for(char i = start; i <= end; i++) { m.insert(std::make_pair(i, type)); }
#define DEBUG false



namespace Utils {
//void log_fn_tree(Node* fn_node) {
//  if (!fn_node)
//    return;
//
//  if (fn_node->nodetype == DECLARATION) 
//  {
//    DeclarationNode *lol = dynamic_cast<DeclarationNode *>(fn_node);
//    std::cout << lol->decl_name << " " << lol->decl_type << " end" << std::endl;
//    log_fn_tree(lol->next);
//
//    if (lol->m_stmts) 
//    {
//      log_fn_tree(lol->m_stmts);
//    }
//  }
//
//  if (fn_node->nodetype == STATEMENT) 
//  {
//    StatementNode *s = dynamic_cast<StatementNode *>(fn_node);
//    log_fn_tree(s->m_stmt_decl->next);
//  }
//
//  return;
//}


    template <typename T>
    inline void logger(const T& txt) {
#if DEBUG
        std::cout << txt << std::endl; 
#else
        return;
#endif
    }

}
