#include "../include/datatypes.hpp"
#include <string>


DATA_TYPE DataType::number_type_from_literal(const std::string &literal) {
      if(!literal.find("."))
            return DATA_TYPE::TYPE_INT;

      return DATA_TYPE::TYPE_DECI;
}
