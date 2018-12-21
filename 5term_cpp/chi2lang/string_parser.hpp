#pragma once

#include <vector>
#include <cassert>
#include "ltoken_type.hpp"
#include "ltoken.hpp"

using std::vector;

bool IsLetter(char c) {
    return c >= 'a' && c <= 'z';
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

bool IsBinOp(char c) {
    return c == '+' ||
           c == '-' ||
           c == '*' ||
           c == '/' ||
           c == '%' ||
           c == '&' ||
           c == '|' ||
           c == '^' ||
           c == '<' ||
           c == '=' ||
           c == ':';
}

bool IsBrace(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}';
}


template <typename Iter>
void AddToken(vector<std::string>& tokens, Iter& first, Iter& second) {
    tokens.emplace_back(std::string(first, second));
    first = second;
}

//template <typename Iter>
//void AddToken(vector<std::string>& tokens, const Iter& first, const Iter& second) {
//    tokens.emplace_back(std::string(first, second));
//    first = second;
//}

vector<std::string> StringToStrTokens(const std::string& str) {
    vector<std::string> tokens;
    auto first = str.begin(), second = str.begin();
    while (first != str.end()) {
        if (second == str.end()) {
            AddToken(tokens, first, second);
        } else if (first == second) {
            ++second;
        } else if (std::isspace(*first)) {
            ++first;
            ++second;
        } else if (IsLetter(*first)) {
            if (IsLetter(*second)) {
                ++second;
            } else {
                AddToken(tokens, first, second);
            }
        } else if (IsDigit(*first)) {
            if (IsDigit(*second)) {
                ++second;
            } else {
                AddToken(tokens, first, second);
            }
        } else if (IsBinOp(*first) || IsBrace(*first) || *first == ';') {
            if (*first == ':') {  // CASE op :=
                assert(*second == '=');
                ++second;
            }
            AddToken(tokens, first, second);
//            if (*first == '-' && (tokens.empty() || tokens.back() == "(")) {  // CASE unary minus
//                tokens.emplace_back("neg");
//                first = second;
//            } else {
//            }
        } else {
            throw std::runtime_error("Invalid input");
        }
    }
    return tokens;
}

vector<Token> StrTokensToTokens(const vector<std::string>& strtokens) {
    vector<Token> tokens;
    for (const auto& st : strtokens) {
        try {
            tokens.emplace_back(token_types.at(st));
        } catch (std::out_of_range&) {
            if (IsDigit(st[0])) {
                tokens.emplace_back(TokenType::CONST, std::stoi(st));
            } else {
                tokens.emplace_back(TokenType::NAME);
            }
        }
    }
    return tokens;
}

void NormalizeTokens(vector<Token>& tokens) {
    for (int i = 0; i < tokens.size() - 1; ++i) {
        if (tokens[i + 1].type == TokenType::OPEN_PAR
            && (tokens[i].type == TokenType::CONST || tokens[i].type == TokenType::NAME)) {
            tokens.insert(tokens.begin() + i + 1, Token(TokenType::MULT));
        } else if (tokens[i + 1].type == TokenType::NAME && tokens[i].type == TokenType::CONST) {
            tokens.insert(tokens.begin() + i + 1, Token(TokenType::MULT));
        }
    }
}


vector<Token> ParseStringToTokens(const std::string& str) {
    auto str_tokens = StringToStrTokens(str);
    auto tokens = StrTokensToTokens(str_tokens);
    NormalizeTokens(tokens);
    return tokens;
}
