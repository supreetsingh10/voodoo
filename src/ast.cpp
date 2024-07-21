#include "../include/ast.hpp"
#include <map>
#include <utility>

ReturnType TypeNode::valid_return_type(const std::string& ret_type) {
    std::map<std::string, ReturnType> m_RetTypes;
    m_RetTypes.insert(std::make_pair("void", TYPE_VOID));
    m_RetTypes.insert(std::make_pair("string", TYPE_STRING));
    m_RetTypes.insert(std::make_pair("char", TYPE_CHAR));
    m_RetTypes.insert(std::make_pair("int", TYPE_INT));
    m_RetTypes.insert(std::make_pair("float", TYPE_FLOAT));

    auto val = m_RetTypes.find(ret_type);

    if (val == m_RetTypes.end()) {
       return TYPE_INVALID; 
    }

    return val->second;
}




