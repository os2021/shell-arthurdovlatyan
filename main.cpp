


#include <iostream>
#include <string>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <cstring>
#include <vector>
#include <algorithm>

static const int sudoid = 0;
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " "

// Get current working directory
std::string getCurrentDir() {
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    return cwd;
}

// print current working directory
void printDir(const std::string& cwd) {
    std::cout << cwd;
    return;
}



// Print $ if user print # if root
void printUser() {
    if (getuid() == sudoid) {
        std::cout << "#";
    } else {
        std::cout << "$";
    }
}
bool takeInput(std::string& command) {
    std::getline(std::cin,command);
    if (!command.empty()) {
        return true;
    }
    return false;
}

// Split the input line into arguments
std::vector<std::string> splitLine(std::string& commandLine) {
    std::vector<std::string> tokens;
    char *token;
    char* line = const_cast<char*>(commandLine.c_str());
    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens.push_back(token);
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    return tokens;
}

int shell_launch(std::vector<std::string>& commands) {
    
}


char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};


int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int my_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("");
        }
    }
    return 1;
}

int my_help(char **args)
{
    printf("The following are built in:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

int my_exit(char **args)
{
    return 0;
}

int (*builtin_func[]) (char **) = {
        &my_cd,
        &my_help,
        &my_exit
};

int executeCommand(std::vector<std::string>& commands) {
    std::vector<char* > cstring;
    cstring.reserve(commands.size());
    for(size_t i = 0; i < commands.size(); ++i)
        cstring.push_back(const_cast<char*>(commands[i].c_str()));
    if (cstring[0] == NULL) {
        return 1;
    }
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(cstring[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(cstring.data());
        }
    }
    pid_t pid, wpid;
    int status;
    std::vector<char*> cstrings;
    bool isBackground;
    if (commands.back() == "&") {
        isBackground = true;
        commands.pop_back();
    }
    cstrings.reserve(commands.size());
    for(size_t i = 0; i < commands.size(); ++i)
        cstrings.push_back(const_cast<char*>(commands[i].c_str()));
    pid = fork();
    if (pid == 0) {
            // Child process
            if (execvp(cstrings[0], &cstrings[0]) == -1) {
                perror("Error in Child");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Error forking
            perror("Error while forking");
        } else {
            // Parent process
            // Wait if not child
            if(!isBackground) {
               do {
                    wpid = waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }

    }
    return 1;
}

int main() {
    std::string commandLine;
    std::vector<std::string> args;
    int status;
    do {
        printDir(getCurrentDir());
        printUser();
        if (!takeInput(commandLine)) {
            continue;
        }
        args = splitLine(commandLine);
        status = executeCommand(args);

        commandLine.clear();
        args.clear();
    } while (status);

}