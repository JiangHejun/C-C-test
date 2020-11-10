#include <iostream>
#include <string>

int main() {
    std::string s = "jhj";

    std::string a;
    a.assign(s);

    std::string b;
    b = s;

    s.clear();
    std::cout << "s string: " << s << std::endl;
    std::cout << "assign string: " << a << std::endl;
    std::cout << "= string: " << b << std::endl;

    return 0;
}