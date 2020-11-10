#include <stdio.h>
#include <unistd.h>
#include <string>

#include <iostream>

std::string GetCwd(){
    char dir[255];
    getcwd(dir, sizeof(dir));
    return std::string(dir);
}

int main() {
    std::cout << GetCwd() << std::endl;
    return 0;
}