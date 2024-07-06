#include <string> 


// TODO Empty for now. 
enum ExprType {
}; 

class ExprNode {
    public:
        ExprNode(); 
        ~ExprNode(); 

        ExprType m_etype; 
        ExprNode* m_pright, *m_pleft; 
        int m_intvalue; 
};

class DeclarationNode {
    public:
        DeclarationNode(); 
        ~DeclarationNode(); 

        std::string m_sname; 
        ExprNode* m_pvalue; 
        DeclarationNode* m_pnext; 
};

// TODO To add more statement types. 
enum StatementType {
    STMT_DECL,
};

class StatementNode {
    public: 
        StatementNode(); 
        ~StatementNode(); 

        StatementType m_eType; 
};

