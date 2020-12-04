#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

std::string GetCwd(const char* path = NULL) {
    char dir[4096];
    if (path == NULL) {
        if (getcwd(dir, sizeof(dir)) == NULL) {
            perror("GetCwd getcwd");
            return "./";
        }
    } else {
        if (realpath(path, dir) == NULL) {
            perror("GetCwd realpath");
            return path;
        }
    }
    return dir;
}

int main() {
    std::cout << GetCwd() << std::endl;
    std::cout << GetCwd("./") << std::endl;
    std::cout << GetCwd("./main") << std::endl;
    std::cout << GetCwd("./main.cpp") << std::endl;
    return 0;
}