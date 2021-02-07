#include <ami/ami.hpp>
#include <iostream>

int main() {
    // basic math expressions
    std::string expression("5^-1*10");
    std::string expression2("4*pi*(5^2)");
    std::string functions("sqrt(25)");
    auto out = ami::eval(expression);
    auto out2 = ami::eval(expression2);
    std::cout << expression << ": " << out << '\n'
              << expression2 << ": " << out2 << '\n'
              << "pi"
              << ": " << ami::eval("pi") << '\n'
              << functions << ": " << ami::eval(functions) << '\n';
}
