# Simple Shell

A command line shell program(rysh) with some built-in command functionality. Built to teach myself more about some lower level Unix processes.

Coded in C++ using skills learned from EECS 678. 

### Functionality
 - Support for basic commands (ls, pwd, echo, wc, grep)
 - Background execution using '&':
 `rysh> sleep 10 &` will run in background 
 - Pipes(|) and redirects(<, >, >>):
 `rysh> ls -l | wc -l` will pipe output of ls -l to input of wc -l
 - Kill and list running jobs:
 `rysh> josb` will list jobs and ids, and `rysh> kill 1` will kill job at 1(id)
 - Set and print system variables:
 `rysh> export PWD=usr/bin` sets $PWD to "usr/bin" and `rysh> echo $PWD` will print the $PWD value

### Running the Shell
To run rysh, navigate to the src folder and run `make`. Then run `./rysh`, and the shell program will be up and running.
To quit, type quit, or exit.
