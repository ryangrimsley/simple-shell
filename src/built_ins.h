#ifndef BUILT_INS_H
#define BUILT_INS_H

#include <string>
#include <vector>
#include <memory>
#include "command_tree.h"

enum class JobState {
    RUNNING,
    COMPLETED,
    KILLED
};

struct Job {
    int jobID;
    std::string command;
    pid_t pid; // Process ID
    JobState state; // Track the state of the job
};

bool parseExportCommand(const std::string& arg, std::string& key, std::string& value);

Job* findJobByID(int jobID);

void executeBuiltIn(const std::shared_ptr<TreeNode>& node);

void printJobs();

std::string expandVariables(const std::string& str);

void builtInJobs();

#endif
