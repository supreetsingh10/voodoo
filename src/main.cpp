#include "../include/lexer.hpp"
// tokens can be of type identifier, operator, literals.
// Abstract class, where the token types can change. 
// Tokens can be of type Interger, Identifier, Literal 
#define TOKENSHOWER false
#if TOKENSHOWER
#include <iostream>
#endif

#include "../include/parser.hpp"

// start reading the file, peak the next token / word, decide it's type, then decide to keep going or terminate the reading. 
int main() {
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
