#include "built_ins.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

// help to split up the export command
bool parseExportCommand(const std::string& arg, std::string& key, std::string& value) {
    size_t equalPos = arg.find('=');
    if (equalPos != std::string::npos) {
        key = arg.substr(0, equalPos);
        value = arg.substr(equalPos + 1);
        return true;
    }
    return false;
}
//helper function to expand so i can expand env vars
std::string expandVariables(const std::string& str) {
    std::string result;
    std::size_t start = 0;

    while (start < str.size()) {
        std::size_t dollarPos = str.find('$', start);
        if (dollarPos == std::string::npos) {
            result += str.substr(start);  // Add remaining part
            break;
        }
        
        result += str.substr(start, dollarPos - start);  // Add part before $

        // Find the variable name
        std::size_t end = dollarPos + 1;
        while (end < str.size() && (isalnum(str[end]) || str[end] == '_')) {
            ++end;
        }

        std::string varName = str.substr(dollarPos + 1, end - dollarPos - 1);
        const char* varValue = getenv(varName.c_str());
        if (varValue) {
            result += varValue;  // Add the value of the variable
        }

        start = end;  // Move past the variable
    }

    return result;
}

void executeBuiltIn(const std::shared_ptr<TreeNode>& node){
    // execute whichever built in is called, check builtins.h (find if kill is one)
    if (node->command == "echo"){
        
        // if first arg has $ meaning its an envirnoment variable, print env variable
        if (node->args[0][0] == '$'){
            //get env, convert arg[0] to string that excludes the $ then convert to char*
            char* envVar = std::getenv((node->args[0].substr(1)).c_str());
            if (envVar != nullptr){
                std::cout << envVar << std::endl;
            }else{
                std::cout << "Error getting environment variable" << std::endl;
            }
        }else{
        // loop through args and print them all to stdout
        for (const auto& arg : node->args){

            std::cout << arg << " ";
        }
        std::cout << std::endl;
        }

        return;
    // if pwd, call getcwd and print result, 
    } else if (node->command == "pwd") {
        char buffer[1024];
        if (getcwd(buffer, 1024) != nullptr){
            std::cout << buffer << std::endl;
        } else{
            std::cerr << "Error getting current directory" << std::endl;
        }
        return;
    // if cd, chdir
    } else if (node->command == "cd") {
        const char* targetDir;
        // if args empty, go to home
        if (node->args.empty()){
            targetDir = getenv("HOME");
            if (targetDir == nullptr){
                std::cerr << "Error: HOME environment variable not set." << std::endl;
            }
        }else{
            targetDir = node->args[0].c_str();
        }
        //assume first argument is path name
        if (chdir(targetDir) == -1){
            std::cerr << "Error changing directory" << std::endl;
        }else{
            //update $PWD to be cwd
            char cwd[1024];
            if (getcwd(cwd, 1024) != nullptr){
                setenv("PWD", cwd, 1);
            }else{
                std::cerr << "Error getting current working directory" << std::endl;
            }
        }
        
        return;
    } else if (node->command == "export") {
        if (node->args.empty()) {
            std::cerr << "Error: No arguments provided for export." << std::endl;
            return;
        }

        for (const auto& arg : node->args) {
            std::string key, value;

            // parse the key=value pair
            if (!parseExportCommand(arg, key, value)) {
                std::cerr << "Error: Invalid format for export. Expected KEY=value." << std::endl;
                continue;  // skip invalid argument
            }

            // set the environment variable
            std::string expandedValue = expandVariables(value);
            if (setenv(key.c_str(), expandedValue.c_str(), 1) == -1) {
                std::cerr << "Error: Failed to set environment variable " << key << std::endl;
            }
        }
        return;
    }else if (node->command == "quit" || node->command == "exit"){
        // if quit or exit command, exit
        exit(0);
    } 
}