#pragma once 

#include <string>

enum DATA_TYPE
{
    TYPE_VOID,
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_DECI,
    TYPE_NONE,
    TYPE_INVALID,
    TYPE_USER_DEF,
};

class DataType
{
public: 
    DataType(): m_data_value("") {}

    DataType(DATA_TYPE dt): m_data_type(dt){}
    DataType(const std::string& d): m_data_value(d) {}

    virtual ~DataType() {}

    std::string m_data_value; 
    DATA_TYPE m_data_type;
};
