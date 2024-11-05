# compiler
CXX = g++

# compiler flags
CXXFLAGS = -Wall -std=c++11

# source files
SRCS = execute.cpp command_tree.cpp parser.cpp built_ins.cpp 

# object files
OBJS = $(SRCS:.cpp=.o)

# target executable
rysh: $(OBJS)
	$(CXX) $(CXXFLAGS) -o rysh $(OBJS)

# rule to create object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# clean rule
clean:
	rm -f $(OBJS) rysh 
