#include <stdio.h>

#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> map;
    map["jhj"] = 100;
    printf("%d\n", map["jhj"]);
    map["jhj"] = 99;
    printf("%d\n", map["jhj"]);
    map["hwq"] = 10;
    printf("%d\n", map["hwq"]);
    printf("%d\n", map["jjj"]);
    printf("%lu\n", map.size());
    return 0;
}