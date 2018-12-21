#pragma once

#include <cassert>
#include <unordered_map>

#include "ltoken_type.hpp"
#include "ltoken.hpp"
#include "node.hpp"

#define NewPtr(type) std::make_unique<Node>(type)

class RecursiveParser {
public:
    RecursiveParser() = default;
    Nptr Parse(const vector<Token>& ts) {
        tokens = ts;
        iter = tokens.begin();
        return ReadProgram();
    }

private:
    const int MAX_PRIOR = 10;
    typedef vector<Token>::const_iterator Iter;
    Iter iter;
    size_t name_count;
    std::unordered_map<std::string, int> names;
    vector<Token> tokens;

    Nptr ReadProgram() {
        auto root = ReadInstruction();
        assert(iter == tokens.end());
        return root;
    }

    Nptr ReadInstruction() {
        if (iter == tokens.end()) {
            return nullptr;
        }
        if (iter->type == TokenType::ENDINSTR) {
            Forward();
            return ReadInstruction();
        }
        Token token = *iter;
        Nptr node = NewPtr(token.type);
        if (token.type == TokenType::DEF) {
            Forward();
            node->value = AssignName(token.name);
            Forward();
            ReadAssertNextType(TokenType::OPEN_BRACE);
            node->left = ReadInstruction();
            ReadAssertNextType(TokenType::CLOSE_BRACE);
        }
        else if (token.type == TokenType::IF || token.type == TokenType::WHILE) {
            node->left = NewPtr(TokenType::IF_LEFT);
            node->right = NewPtr(TokenType::IF_RIGHT);
            Forward();

            ReadAssertNextType(TokenType::OPEN_PAR);
            node->left->left = ReadExpr();
            ReadAssertNextType(TokenType::CLOSE_PAR);
            ReadAssertNextType(TokenType::OPEN_BRACE);
            node->left->right = ReadInstruction();
            ReadAssertNextType(TokenType::CLOSE_BRACE);
            if (token.type == TokenType::IF) {
                ReadAssertNextType(TokenType::ELSE);
                ReadAssertNextType(TokenType::OPEN_BRACE);
                node->right->left = ReadInstruction();
                ReadAssertNextType(TokenType::CLOSE_BRACE);
            } else {
                node->right->left = nullptr;
            }

            node->right->right = ReadInstruction();
        }
        else if (token.type == TokenType::DECLVAR) {
            Forward();
            node->value = AssignName(iter->name);
            Forward();
            ReadAssertNextType(TokenType::ENDINSTR);
            node->right = ReadInstruction();
        }
        else {
            node->type = TokenType::NOP;  // does nothing
            node->left = ReadExpr();
            node->right = ReadInstruction();
        }
        return node;
    }

    Nptr ReadExpr() {
        auto look_forward_iter = iter;
        int par_balance = 0;
        while (par_balance >= 0 && look_forward_iter->type != TokenType::ENDINSTR && look_forward_iter != tokens.end()) {
            ++look_forward_iter;
        }
        return ReadExpr(look_forward_iter);
    }

    Nptr ReadExpr(Iter delimiter) {  // todo Assign - out of expression to forbid lines "a+b;"
        if (iter == delimiter || iter->type == TokenType::ENDINSTR) {
            return nullptr;
        }
        if (iter + 1 == delimiter) {
            return ReadToken();
        }
        if (iter->type == TokenType::OPEN_PAR) {
            Forward();
            auto node = ReadExpr(delimiter - 1);
            ReadAssertNextType(TokenType::CLOSE_PAR);
            return node;
        }
        if (iter->type == TokenType::PRINT) {
            Nptr node = NewPtr(iter->type);
            Forward();
            ReadAssertNextType(TokenType::OPEN_PAR);
            node->left = ReadExpr(delimiter - 1);
            ReadAssertNextType(TokenType::CLOSE_PAR);
            ReadAssertNextType(TokenType::ENDINSTR);
            return node;
        }

        auto next_op = delimiter;
        for (int prior = MIN_PRIOR; prior <= MAX_PRIOR; ++prior) {
            int par_balance = 0;
            for (auto look_forward_iter = iter; look_forward_iter != delimiter; ++look_forward_iter ) {
                TokenType type = look_forward_iter->type;
                if (type == TokenType::OPEN_PAR) {
                    ++par_balance;
                } else if (type == TokenType::CLOSE_PAR) {
                    --par_balance;
                } else if (par_balance == 0 && priority[type] == prior) {
                    if ((!associativity[type] && next_op == delimiter) || associativity[type]) {
                        next_op = look_forward_iter;
                    }
                }
            }
            if (next_op != delimiter) {
                Nptr node = NewPtr(next_op->type);
                node->value = next_op->value;
                node->left = ReadExpr(next_op);
                if (next_op->type != TokenType::PRINT) {
                    Forward();
                    node->right = ReadExpr(delimiter);
                }
                return node;
            }
        }
        throw std::runtime_error("Bad expression");
    }

    Nptr ReadToken() {
        Nptr node = NewPtr(iter->type);
        if (iter->type == TokenType::NAME) {
            node->value = names[iter->name];
        } else if (iter->type == TokenType::CONST) {
            node->value = iter->value;
        }
        Forward();
        return node;
    }

    int AssignName(const std::string& token) {
        if (names.find(token) == names.end()) {
            names[token] = name_count;
            return name_count++;
        } else {
            return names[token];
        }
    }

    void Forward() {
        ++iter;
    }

    void ReadAssertNextType(TokenType type) {
        if (iter->type == type) {
            ++iter;
        } else {
            throw std::runtime_error("Bad token: " + std::to_string(int(type)));
        }
    }
};
