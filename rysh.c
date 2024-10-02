#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define TK_BUFF_SIZE 64
#define TOK_DELIM " \t\r\n"
#define RED "\033[0;31m"
#define COLOR_RESET "\e[0m"

char *read_line();
char **split_line(char *);
int rysh_exit(char **);
int rysh_execute(char **);

int rysh_execute(char **args) {
	// Initialize child process id and status to 
	pid_t cpid;
	int status;
	// if first arg is exit, exit
	if (strcmp(args[0], "exit") == 0) {
		return rysh_exit(args);
	}

	cpid = fork();

	if (cpid == 0) {
		if (execvp(args[0], args) < 0)
			printf("rysh: command not found: %s\n", args[0]);
		exit(EXIT_FAILURE);

	} else if (cpid < 0)
		printf(RED "Error forking" COLOR_RESET "\n");
	else {
		waitpid(cpid, & status, WUNTRACED);
	}
	return 1;
}

int rysh_exit(char **args) {
	return 0;
}

char **split_line(char * line) {
	int buffsize = TK_BUFF_SIZE, position = 0;
	char **tokens = malloc(buffsize * sizeof(char *));
	char *token;

	if (!tokens) {
		fprintf(stderr, "%srysh: Allocation error%s\n", RED, COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	token = strtok(line, TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= buffsize) {
			buffsize += TK_BUFF_SIZE;
			tokens = realloc(tokens, buffsize * sizeof(char * ));

      		if (!tokens) {
        		fprintf(stderr, "%srysh: Allocation error%s\n", RED, COLOR_RESET);
        		exit(EXIT_FAILURE);
      		}
    	}
    token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;

	return tokens;
}

char *read_line() {
	int buffsize = 1024;
	int position = 0;
	char *buffer = malloc(sizeof(char) * buffsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "%srysh: Allocation error%s\n", RED, COLOR_RESET);
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getchar();
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
    	position++;

    	if (position >= buffsize) {
			buffsize += 1024;
			buffer = realloc(buffer, buffsize);

      		if (!buffer) {
        		fprintf(stderr, "rysh: Allocation error\n");
        		exit(EXIT_FAILURE);
      		}
    	}
	}
}

void loop() {
	char *line;
	char **args;
	int status = 1;

	do {
		printf("rysh > ");
		line = read_line();
    	args = split_line(line);
    	status = rysh_execute(args);
    	free(line);
		free(args);
	} while (status);
}

int main() {
	loop();
	return 0;
}
