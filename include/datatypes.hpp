#pragma once 

#include <string>

enum DATA_TYPE
{
    TYPE_VOID,
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_DECI,
    TYPE_USER_DEF,

    TYPE_INVALID,
    TYPE_NONE,
};

class DataType
{
public: 

    DataType(): m_data_value("") {}
    DataType(DATA_TYPE dt): m_data_type(dt) {}
    DataType(const std::string& d): m_data_value(d) {}
    DataType(const std::string& d, const DATA_TYPE& dt): m_data_value(d), m_data_type(dt) {}

    virtual ~DataType() {}

    std::string m_data_value; 
    DATA_TYPE m_data_type;

    static DATA_TYPE number_type_from_literal(const std::string&);
};
