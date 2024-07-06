#include <string> 

struct Error {
    std::string error_text; 
    std::string faulty_code_text; 
    int line_number; 

    Error(std::string err_text, std::string code, int number) 
        : error_text(err_text), faulty_code_text(code), line_number(number) 
    {
    }
};
