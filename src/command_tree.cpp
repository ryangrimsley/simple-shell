#include "command_tree.h"
#include <iostream>

std::shared_ptr<TreeNode> createTreeNode(Token token){
    // command is the value, args is empty arr for now, type is type, and left and right children start as null
    return std::make_shared<TreeNode>(TreeNode{token.value, {}, token.type, nullptr, nullptr, false});
}

std::shared_ptr<TreeNode> buildParseTree(const std::vector<Token> tokens, size_t& pos) {
    size_t tokensLength = tokens.size();
    if (pos >= tokensLength) return nullptr;

    // create the current command node
    std::shared_ptr<TreeNode> current = createTreeNode(tokens[pos]);
    pos++;

    // collect arguments
    while (pos < tokensLength && tokens[pos].type == TokenType::COMMAND) {
        current->args.push_back(tokens[pos].value);
        pos++;
    }

    // process operators
    while (pos < tokensLength) {
        if (tokens[pos].type == TokenType::PIPE) {
            pos++; // skip '|'
            // create new operator node
            std::shared_ptr<TreeNode> operatorNode = createTreeNode({"|", TokenType::PIPE});
            operatorNode->leftChild = current;
            operatorNode->rightChild = buildParseTree(tokens, pos);
            current = operatorNode;
        } else if (tokens[pos].type == TokenType::REDIRECT_OUTPUT) {
            pos++; // skip '>'
            // create new operator node
            std::shared_ptr<TreeNode> operatorNode = createTreeNode({">", TokenType::REDIRECT_OUTPUT});
            operatorNode->leftChild = current;
            // next token is the filename
            std::shared_ptr<TreeNode> filenameNode = createTreeNode(tokens[pos]);
            pos++;
            operatorNode->rightChild = filenameNode;
            current = operatorNode;
        } else if (tokens[pos].type == TokenType::APPEND_OUTPUT) {
            pos++; // skip '>>'
            std::shared_ptr<TreeNode> operatorNode = createTreeNode({">>", TokenType::APPEND_OUTPUT});
            operatorNode->leftChild = current;
            // next token is the filename
            std::shared_ptr<TreeNode> filenameNode = createTreeNode(tokens[pos]);
            pos++;
            operatorNode->rightChild = filenameNode;
            current = operatorNode;
        } else if (tokens[pos].type == TokenType::REDIRECT_INPUT) {
            pos++; // skip '<'
            std::shared_ptr<TreeNode> operatorNode = createTreeNode({"<", TokenType::REDIRECT_INPUT});
            operatorNode->leftChild = current;
            // next token is the filename
            std::shared_ptr<TreeNode> filenameNode = createTreeNode(tokens[pos]);
            pos++;
            operatorNode->rightChild = filenameNode;
            current = operatorNode;
        } else if (tokens[pos].type == TokenType::BACKGROUND) {
            current->isBackground = true;
            pos++;
        } else {
            break;
        }
    }
    return current;
}


// Helper function to print the command tree
void printCommandTree(const std::shared_ptr<TreeNode>& node, int depth) {
    if (!node) return;

    // Print the current node with indentation based on depth
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";  // Indent to show tree structure
    }
    std::cout << "Command: " << node->command;

    // Print arguments
    if (!node->args.empty()) {
        std::cout << " Args: [";
        for (size_t i = 0; i < node->args.size(); ++i) {
            std::cout << node->args[i];
            if (i < node->args.size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }

    // Print if the command is set to run in the background
    if (node->isBackground) {
        std::cout << " (Background)";
    }

    std::cout << std::endl;

    // Recursively print left and right branches (pipes, redirections, etc.)
    if (node->leftChild) {
        printCommandTree(node->leftChild, depth + 1);
    }
    if (node->rightChild) {
        printCommandTree(node->rightChild, depth + 1);
    }
}
