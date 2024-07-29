#include "../include/lexer.hpp"

#define TOKENSHOWER true
#if TOKENSHOWER
#include <iostream>
#endif

#include "../include/parser.hpp"

// start reading the file, peak the next token / word, decide it's type, then decide to keep going or terminate the reading. 
int main() 
{
    Lexer lexy = Lexer(); 

    lexy.read_file("./some.hen");
    Token::initialize_map(); 
    lexy.scan(); 

#if TOKENSHOWER
    for (auto t : *lexy.m_vptokens)
        std::cout << t->get_value() << " " << t->get_type() << std::endl; 
#endif

    Parser parser = Parser(); 
    parser.set_input_stream(*lexy.m_vptokens);

    parser.parse();


    return 0; 
}
