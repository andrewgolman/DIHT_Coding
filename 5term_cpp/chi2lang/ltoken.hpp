#pragma once

#include <map>
#include <utility>

#include "ltoken_type.hpp"

struct Token {
    TokenType type;
    double value;
    std::string name;

    Token(TokenType _type, double _value = 0) : type(_type), value(_value) {}
    Token(const Token& token) = default;
};
