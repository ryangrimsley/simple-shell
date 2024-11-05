#ifndef EXECUTE_H
#define EXECUTE_H

#include "command_tree.h"
#include "built_ins.h"
#include <memory>

void executeCommand(const std::shared_ptr<TreeNode>& node);

#endif