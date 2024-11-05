#include <vector>
#include <iostream>
#include <string>
#include <unistd.h> 
#include <sstream>
#include <signal.h>
#include "parser.h"
#include "command_tree.h"
#include "built_ins.h"
#include <sys/wait.h>
#include <fcntl.h>
#include "execute.h"
#include <algorithm>

using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::endl;

class JobManager {
private:
    std::vector<Job> jobList;
    int nextJobID;

public:
    JobManager() : nextJobID(1) {}

    void addJob(const std::string& command, pid_t pid) {
        Job newJob{nextJobID++, command, pid, JobState::RUNNING};
        jobList.push_back(newJob);
        std::cout << "Background job started: [" << newJob.jobID << "] " << pid << " " << command << std::endl;
    }

    void completeJob(int jobID) {
        for (auto& job : jobList) {
            if (job.jobID == jobID && job.state == JobState::RUNNING) {
                job.state = JobState::COMPLETED;
                std::cout << "Completed: [" << jobID << "] " << job.pid << " " << job.command << std::endl;
            }
        }
    }

    void killJob(int jobID) {
        for (auto& job : jobList) {
            if (job.jobID == jobID) {
                kill(job.pid, SIGTERM); // Send term signal
                job.state = JobState::KILLED;
                std::cout << "Job " << jobID << " killed." << std::endl;
                return;
            }
        }
        std::cout << "No job found with ID " << jobID << std::endl;
    }

    void checkCompletedJobs() {
        for (auto& job : jobList) {
            int status;
            if (job.state == JobState::KILLED){
                removeJob(job.jobID);
            }
            pid_t result = waitpid(job.pid, &status, WNOHANG);
            if (result == job.pid) { // If the process has finished
                if (WIFEXITED(status)) {
                    completeJob(job.jobID);
                }
            }
        }
    }
    void removeJob(int jobID) {
        // Remove job from the list using the jobID
        jobList.erase(std::remove_if(jobList.begin(), jobList.end(),
                                   [jobID](const Job& job) { return job.jobID == jobID; }),
                    jobList.end());
    }

    void printJobs() {
        if (jobList.empty()){
            std::cout << "No jobs running." << std::endl;
            return;
        }
        for (const auto& job : jobList) {
            std::cout << "[" << job.jobID << "] " << job.pid << " " << job.command << endl;
        }
    }
};

void executeCommand(const std::shared_ptr<TreeNode>& node){
    // check if any args are env vars and expand them
    for (auto& arg : node->args){
        if (arg[0] == '$'){
            arg = expandVariables(arg);
        }
    }
    // expand if its an environment variable
    std::vector<char*> args;
    args.push_back(const_cast<char*>(node->command.c_str()));
    for (auto& arg : node->args){
        args.push_back(const_cast<char*>(arg.c_str()));
    }
    //add null ptr so it knows where it ends
    args.push_back(nullptr);
    //run command
    execvp(args[0], args.data());
    perror("execvp");
    exit(1);
}

void executeParseTree(const std::shared_ptr<TreeNode>& node, JobManager& jobManager) {
    if (!node) return;

    if (node->command == "|") {
        // handle pipe
        int fds[2];
        if (pipe(fds) == -1) {
            perror("pipe");
            exit(1);
        }

        pid_t left_pid = fork();
        if (left_pid == 0) {
            // left child process: execute left command, write to pipe
            close(fds[0]); // close unused read end
            if (dup2(fds[1], STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            close(fds[1]);
            executeParseTree(node->leftChild, jobManager);
            exit(0);
        }

        pid_t right_pid = fork();
        if (right_pid == 0) {
            // right child process: read from pipe, execute right command
            close(fds[1]); // close unused write end
            if (dup2(fds[0], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            close(fds[0]);
            executeParseTree(node->rightChild, jobManager);
            exit(0);
        }

        // parent process: close pipe and wait for children
        close(fds[0]);
        close(fds[1]);
        waitpid(left_pid, nullptr, 0);
        waitpid(right_pid, nullptr, 0);

    } else if (node->command == ">" || node->command == ">>" || node->command == "<") {
        // handle redirection
        pid_t pid = fork();
        if (pid == 0) {
            int fd;
            std::string filename = node->rightChild->command; // filename is in rightChild's command

            if (node->command == ">") {
                fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            } else if (node->command == ">>") {
                fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            } else if (node->command == "<") {
                fd = open(filename.c_str(), O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }
            close(fd);
            // now execute the leftChild
            executeParseTree(node->leftChild, jobManager);
            exit(0);
        } else {
            // parent process
            waitpid(pid, nullptr, 0);
        }

    } else {
        // execute command node
        if (node->command == "echo" || node->command == "pwd" || node->command == "export" || node->command == "quit" || node->command == "exit" || node->command == "cd") {
            executeBuiltIn(node);
            return;
        }
        if (node->command == "jobs") {
            jobManager.printJobs();
            return;
        }
        if (node->command == "kill" && !node->args.empty()) {
            int jobID = std::stoi(node->args[0]);  // assume the first argument is the job ID
            jobManager.killJob(jobID);
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            executeCommand(node);
            exit(0);
        } else {
            if (node->isBackground) {
                jobManager.addJob(node->command, pid);
            }
            if (!node->isBackground) {
                waitpid(pid, nullptr, 0);
            }
        }
    }
}




int main(){
    //set running condition to true
    cout << "Welcome..." << endl;
    JobManager jobManager;
    while (true){
        jobManager.checkCompletedJobs();
        string userIn; //string to hold user input
        cout << "quash> ";
        std::getline(cin, userIn); //get user input
        vector<Token> tokens = parseInput(userIn); //parse user input into tokens
        size_t pos = 0; // starting index = 0
        auto commandTree = buildParseTree(tokens, pos); //build tree of commands
        executeParseTree(commandTree, jobManager); //execute commands
    }
}
    

