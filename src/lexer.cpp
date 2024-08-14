#include "../include/lexer.hpp"
#include "../include/source.hpp"
#include <cassert>
#include <memory>
#include "../include/utilities.hpp"

Lexer::~Lexer() 
{
    Utils::logger("Destructed");
    assert(m_vptokens->size() != 0);
    SAFE_RELEASE_VECTOR(*m_vptokens);
}


void Lexer::read_file(const std::string& file_name) 
{
    source_code = std::make_unique<Source>();
    source_code->initialize(file_name);
}


// this should have functions which will check if
// the token generated is a keyword.
// Operator's type
void Lexer::tokenize() 
{
    if(m_pcurrent_token->get_type() == TokenTypes::IDENTIFIER) 
    {
        Identifier* iden = dynamic_cast<Identifier*>(m_pcurrent_token); 
        iden->check_set_keyword();
    } else if (m_pcurrent_token->get_type() == TokenTypes::OPERATOR) {
        Operators* ops = dynamic_cast<Operators*>(m_pcurrent_token); 
        ops->check_set_valid_token_type();
    }

#if DEBUG
    Utils::logger(m_pcurrent_token->get_value());
#endif

    m_vptokens->push_back(m_pcurrent_token->make_copy()); 

    // delete the copied token to prevent memory leak.
    delete m_pcurrent_token; 
    m_pcurrent_token = nullptr;
}

// This scans the file from the start to the end and it goes character by character. 
// The current_char type is checked and set. 
// If the current_char type is valid then it goes forward peeks the next character and goes on in the loop. 
// If the next character is incompatible then it breaks the loop, copies the token and then pushes it. 
void Lexer::scan()
{
    while (source_code->get_file_buffer().good()) {
        char current_char = source_code->get_file_buffer().get(); 
        if(current_char == '\n') {
            ++m_line_number; 
        }
        // EOF character is -1; 
        if ((int)current_char != -1) {
            if (m_pcurrent_token == nullptr) {
               assert((int)current_char != -1);
               TokenTypes current_type = Token::get_type(current_char); 
               switch (current_type) {
                   case IDENTIFIER:
                       m_pcurrent_token = new Identifier(); 
                       m_pcurrent_token->set_type(TokenTypes::IDENTIFIER); 
                       break; 
                   case NUMBER: 
                       m_pcurrent_token = new Numbers(); 
                       m_pcurrent_token->set_type(TokenTypes::NUMBER); 
                       break;
                       // For literals to work
                       // We will need to check if they are initializing with Single or double quotes. 
                       // if single quote, then it is a character, else it is a string. 
                       // We need to enforce the length for single quote character to be 1. 
                       // Even whitespaces will be valid inside of a string / literal. 
                       //
                       // Literal will only stop parsing when 
                    case LITERAL: 
                       m_pcurrent_token = new Literal(); 
                       m_pcurrent_token->set_type(TokenTypes::LITERAL); 
                       break; 
                    case OPERATOR:
#if DEBUG
                       Utils::logger("Operator found");
                       std::cout << current_char << std::endl; 
#endif
                       m_pcurrent_token = new Operators(); 
                       m_pcurrent_token->set_type(TokenTypes::OPERATOR); 
                       break;
                    case WHITESPACE:
#if DEBUG
                       Utils::logger("WHITESPACE found");
#endif
                       m_pcurrent_token = nullptr; 
                       break; 
                    default:
                       m_pcurrent_token = nullptr; 
                       break; 
                   }
            } 

            if(!m_pcurrent_token)
                continue;

            if (source_code->get_file_buffer().peek() != source_code->get_file_buffer().eof()) {
                // if it is not the end. 
                char next_char = source_code->get_file_buffer().peek(); 
                bool incompatible = m_pcurrent_token->parse(current_char, next_char); 

                if (incompatible) {
                    tokenize(); 
                }
            } else {
                // handle the end; 
                if(m_pcurrent_token->parse_end(current_char)) {
                   tokenize(); 
                   break;
                }
            }
        } else {
            // if the current character is an end of file. 
           break;  
        }
    }
}
