#pragma once

enum OperatorEnum {
    NOPE,
    ADDITION,
    SUBTRACTION,
    DIVISION,
    MULTIPLICATION,

    GREATER_THAN,
    LESSER_THAN,
    ASSIGNMENT,
    EQUALS,
    NOT_EQUAL,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,

    RIGHT_PAREN,
    LEFT_PAREN,
    RIGHT_CURLY,
    LEFT_CURLY,
    RIGHT_SQUARE_BRACKET,
    LEFT_SQUARE_BRACKET,
    UNDERSCORE,

    DASH,
    COLON,
    SEMI_COLON,
    AT_RATE,
    DOLLAR,
    QUESTION_MARK,
    STRAIGHT_BAR,
    TILDE,
    BACK_TICK,
    HASH,
    PERCENT,
    EXCLAIMATION_MARK,
    CARET,
    COMMA,
    AMPERSAND,
    BACK_SLASH,
    DOT,
    // These are single and double quotes. 
    // They will be later decided if they are right or left. 

    DOUBLE_QUOTE,
    RIGHT_DOUBLE_QUOTE,
    LEFT_DOUBLE_QUOTE,
    RIGHT_SINGLE_QUOTE,
    LEFT_SINGLE_QUOTE,
};


struct Operator
{
    OperatorEnum operator_enum;
    int precedence;

    // added to fix the compiler warning.
    Operator():operator_enum(NOPE) {
    }

    // added to fix the macro.
    Operator(OperatorEnum op_enum, int prec):
        operator_enum(op_enum), precedence(prec) {
    }

    Operator(const Operator* ops) {
       this->operator_enum = ops->operator_enum;
       this->precedence = ops->precedence; 
    }

    const int get_precedence() const {
        return this->precedence;
    }

    const OperatorEnum get_operator_enum() const {
        return this->operator_enum;
    }
};


#ifndef OperatorGenerator
#define OperatorGenerator(op_enum, preced) Operator(op_enum, int)
#endif
