#ifndef COMMAND_TREE_H
#define COMMAND_TREE_H

#include <vector>
#include <string>
#include <memory>
#include "parser.h"
//struct to represent nodes in tree
struct TreeNode{
    std::string command;
    std::vector<std::string> args;
    TokenType type;
    std::shared_ptr<TreeNode> leftChild;
    std::shared_ptr<TreeNode> rightChild;
    bool isBackground;
};

void printCommandTree(const std::shared_ptr<TreeNode>& node, int depth = 0);
std::shared_ptr<TreeNode> buildParseTree(const std::vector<Token> tokens, size_t& pos);
std::shared_ptr<TreeNode> createTreeNode(Token token);

#endif
