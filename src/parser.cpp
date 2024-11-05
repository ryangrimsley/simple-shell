#include "parser.h"
#include <vector>
#include <string>
#include <sstream>


// parse input by looping through every set of chars seperated by spaces 
// and assigning them to a vector of Tokens with a TokenType and value
std::vector<Token> parseInput(std::string input){
    // create istringstream to easily go through every "word" in input
    std::istringstream iss(input);
    // vector to store Tokens
    std::vector<Token> tokens;
    // token string that will hold tokens as they are iterated through
    std::string token;
    // while iss still has "words" to iterate into token
    while (iss >> token){
        // check for the type of token and assign a Token for it
        if (token == "|"){
            tokens.push_back({token, TokenType::PIPE});
        }else if(token == "<"){
            tokens.push_back({token, TokenType::REDIRECT_INPUT});
        }else if (token == ">"){
            tokens.push_back({token, TokenType::REDIRECT_OUTPUT});
        }else if (token == ">>"){
            tokens.push_back({token, TokenType::APPEND_OUTPUT});
        }else if (token == "&"){
            tokens.push_back({token, TokenType::BACKGROUND});
        }
        // may need to add more types
        else{
            tokens.push_back({token, TokenType::COMMAND});
        }
    }
    return tokens;
}