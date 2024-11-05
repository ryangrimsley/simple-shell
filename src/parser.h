#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

enum class TokenType{
    COMMAND,
    PIPE,
    REDIRECT_OUTPUT,
    REDIRECT_INPUT,
    BACKGROUND,
    APPEND_OUTPUT,
    OTHER
};

struct Token {
    std::string value;
    TokenType type;
};

std::vector<Token> parseInput(std::string input);

#endif