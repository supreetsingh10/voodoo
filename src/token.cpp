#include "../include/token.hpp"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include "../include/utilities.hpp"

std::map<char, TokenTypes> Token::m_symbol_map; 

Token::Token() {
}

Token::~Token() {
    // TODO 
    // this is not clearing up heap memory for now. 
    // check this. 

}

void Token::initialize_map() {
    POPULATE_MAP(m_symbol_map, '0', '9', TokenTypes::NUMBER);
    POPULATE_MAP(m_symbol_map, 'a', 'z', TokenTypes::IDENTIFIER);
    POPULATE_MAP(m_symbol_map, 'A', 'Z', TokenTypes::IDENTIFIER);
    POPULATE_MAP(m_symbol_map, 33, 47, TokenTypes::OPERATOR);
    POPULATE_MAP(m_symbol_map, 58, 64, TokenTypes::OPERATOR);
    POPULATE_MAP(m_symbol_map, 91, 96, TokenTypes::OPERATOR);
    POPULATE_MAP(m_symbol_map, 123, 126, TokenTypes::OPERATOR);

// This will overwrite them as Literal. For now only double quoted literals are being handled. 
// Need to enforce single quoted literals as characters. 
    m_symbol_map.at('"') = TokenTypes::LITERAL; 
    m_symbol_map.at('\'') = TokenTypes::LITERAL; 

    m_symbol_map.insert(std::make_pair(' ', TokenTypes::WHITESPACE));
    m_symbol_map.insert(std::make_pair('\n', TokenTypes::WHITESPACE));
    m_symbol_map.insert(std::make_pair('\r', TokenTypes::WHITESPACE));
    m_symbol_map.insert(std::make_pair('\t', TokenTypes::WHITESPACE));

}


// this will function like this for everyting. 
void Token::set_type(const TokenTypes& type) {
    this->token_type = type; 
}

void Token::set_line_number(const uint32_t line_number) {
    m_line_number = line_number;
}

bool Token::parse_end(char c) {
    this->token_value += c; 
    return true; 
}

const std::string Token::get_value() const {
    return this->token_value; 
}

// this is a getter. 
const TokenTypes Token::get_type() const {
    return this->token_type; 
}

TokenTypes Token::get_type(char c) {
    TokenTypes type = NONE;
    try {
        type = m_symbol_map.at(c); 
    } catch (std::out_of_range o) {
        std::cerr << "Character " << (int)c << " " << c << " is not found " << o.what() << std::endl;  
        assert(!"Character not found");
    }

    return type; 
}

Literal::Literal() {
}

Literal::~Literal() {
}

Literal* Literal::make_copy() {
    return new Literal(this); 
}

// will parse as long as there are quotes. So avoid this.
std::set<TokenTypes> Literal::m_compatible_types; 
bool Literal::parse(char cur, char next) {
    if (cur == '"') {
        // "heelo world" -> "he"llo;
        if (m_stack_valid_quotes.empty()) {
           m_stack_valid_quotes.push(cur); 
        } else if (m_stack_valid_quotes.top() == cur) {
            m_stack_valid_quotes.pop(); 
        } 
    }
    this->token_value += cur; 
    return incompatible_type(next);
}

// Literal has all the types as compatible as long as there are only single 
// This will mark the end if there are no nested quotes
bool Literal::incompatible_type(char next) {
    return m_stack_valid_quotes.empty();
}


Identifier::Identifier() {
}

Identifier::~Identifier() {
}

void Identifier::check_set_keyword() {
    std::set<std::string>::iterator it = m_set_keywords.find(token_value); 
    if(it != m_set_keywords.end()) {
       token_type = KEYWORD; 
    }
}

std::set<TokenTypes> Identifier::m_compatible_types = {TokenTypes::IDENTIFIER, TokenTypes::NUMBER}; 

std::set<std::string> Identifier::m_set_keywords = {
// keywords
    "let", 
    "for", 
    "loop",
    "fn",
    "pack",

// datatypes. 
    "int", 
    "deci", 
    "str", 

//  confiditionals
    "if",
    "elif",
    "else"
}; 


Identifier* Identifier::make_copy() {
     return new Identifier(this); 
}

bool Identifier::parse(char cur, char next) {
    this->token_value += cur; 
    return this->incompatible_type(next); 
}

bool Identifier::incompatible_type(char next_char) {
    if (Token::get_type(next_char) == TokenTypes::OPERATOR) {
       if (next_char == '_')
           return false;
    }

    if(Identifier::m_compatible_types.find(m_symbol_map[next_char]) == m_compatible_types.end())
        return true;

    return false;
}


Numbers::Numbers() {
}

Numbers::~Numbers() {
}


Numbers* Numbers::make_copy() {
    return new Numbers(this); 
}

std::set<TokenTypes> Numbers::m_compatible_types = {TokenTypes::NUMBER};
bool Numbers::parse(char cur, char next) {
    this->token_value += cur; 
    return this->incompatible_type(next); 
}


// TODO Add the compaitble operator types to the classes. 
bool Numbers::incompatible_type(char next) {
    if(Numbers::m_compatible_types.find(m_symbol_map[next]) != m_compatible_types.end())
        return false;

    if(Token::get_type(next) == TokenTypes::OPERATOR) {
        if(next == '.') {
            if(this->decimal_added) {
               assert(!"Multiple decimals found"); 
            }
            this->decimal_added = true; 
            return false;
        } 
    }

    return true;
}

Operators::Operators() {}
Operators::~Operators() {}


// The lower the precedence the important the operator, it will be executed first. 
std::map<std::string, Operator> Operators::OperatorMap = {
    {
        "#", Operator(OperatorEnum::HASH, 1)
    },
    {
        "\\", Operator(OperatorEnum::BACK_SLASH, 1)
    },
    {
        "_", Operator(OperatorEnum::UNDERSCORE, 1)
    },
    {
        "@", Operator(OperatorEnum::AT_RATE, 1)
    },
    {
        "$", Operator(OperatorEnum::DOLLAR, 1)
    },
    {
        ";", Operator(OperatorEnum::SEMI_COLON, 1)
    }, 
    {
        ":", Operator(OperatorEnum::COLON, 1)
    },
    {
        "[", Operator(OperatorEnum::RIGHT_SQUARE_BRACKET, 1)
    },
    {
        "]", Operator(OperatorEnum::LEFT_SQUARE_BRACKET, 1)
    },
    {
        "(", Operator(OperatorEnum::RIGHT_PAREN, 1)
    },
    {
        ")", Operator(OperatorEnum::LEFT_PAREN, 1)
    },
    {
        "{", Operator(OperatorEnum::RIGHT_CURLY, 1)
    },
    {
        "}", Operator(OperatorEnum::LEFT_CURLY, 1)
    },
    {
        "`", Operator(OperatorEnum::BACK_TICK, 1)
    },
    {
        "!", Operator(OperatorEnum::EXCLAIMATION_MARK, 2)
    },
    {
        "*", Operator(OperatorEnum::MULTIPLICATION, 3)
    },
    {
        "/", Operator(OperatorEnum::DIVISION, 3)
    },
    {
        "%", Operator(OperatorEnum::PERCENT, 3)
    },
    {
        "\"", Operator(OperatorEnum::DOUBLE_QUOTE, 3)
    }, 
    {
        "+", Operator(OperatorEnum::ADDITION, 4)
    }, 
    {
        "-", Operator(OperatorEnum::SUBTRACTION, 4)
    },
    {
        ">", Operator(OperatorEnum::GREATER_THAN, 6)
    },
    {
        "<", Operator(OperatorEnum::LESSER_THAN, 6)
    },
    {
        ">=", Operator(OperatorEnum::GREATER_THAN_EQUAL, 6)
    },
    {
        "<=", Operator(OperatorEnum::LESS_THAN_EQUAL, 6)
    },
    {
        "?", Operator(OperatorEnum::QUESTION_MARK, 6)
    },
    {
        "==", Operator(OperatorEnum::EQUALS, 7)
    },
    {
        "!=", Operator(OperatorEnum::NOT_EQUAL, 7)
    },
    {
        "|", Operator(OperatorEnum::STRAIGHT_BAR, 8)
    },
    {
        "~", Operator(OperatorEnum::TILDE, 8)
    },
    {
        "&", Operator(OperatorEnum::AMPERSAND, 8)
    },
    {
        "^", Operator(OperatorEnum::CARET, 9)
    },
    {
        "=", Operator(OperatorEnum::ASSIGNMENT, 14)
    },
    {
        ",", Operator(OperatorEnum::COMMA, 15)
    },
};


bool Operators::parse(char cur, char next) 
{
    token_value += cur; 

    // Check here if the certain operator can only come once, or it can be doubled. 
    // These operators can only come once.
    std::set<char> single_operators = { '{', '}', '(', ')', '[', ']', '?', ','};

    // if it finds any of these characters that can only exist alone, 
    // then it will return true and tokenize them
    // else it will go forward
    if(single_operators.find(cur) != single_operators.end())
        return true;

    return this->incompatible_type(next); 
}

std::set<TokenTypes> Operators::m_compatible_types = {OPERATOR}; 
bool Operators::incompatible_type(char next) 
{
    if(m_compatible_types.find(Token::get_type(next)) == m_compatible_types.end())
        return true;

    return false;
}

bool Operators::check_set_valid_token_type() 
{
    try 
    {
        this->m_operator = OperatorMap.at(token_value);
    }
    catch (std::out_of_range) 
    {
       std::cerr << "Failed the operator " << this->token_value << ", check at line number " << m_line_number << std::endl;
       assert(!"Failed to find the operator"); 
       exit(1);
    }

    return true;
}

Operators* Operators::make_copy() 
{
     return new Operators(this); 
}
