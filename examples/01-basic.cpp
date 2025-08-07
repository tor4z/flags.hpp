#include <vector>
#define FLAGS_IMPLEMENTATION
#include "flags.hpp"
#include <iostream>


int main(int argc, char** argv)
{
    Flags::parse(argc, argv)
        ->with_arg("test", 't', 100, "int test value")
        ->with_arg("args", 'a', std::vector<int>(), "some arg list")
        ->with_opt("option", 'o', "option test")
        ->with_help("basic example")
        ->build();

    std::cout << Flags::arg<int>("test") << "\n";
    std::cout << Flags::args<std::string>().empty() << "\n";
    std::cout << Flags::opt("option") << "\n";

    std::cout << "args: ";
    auto args{Flags::arg<std::vector<int>>("args")};
    for (auto it : args) {
        std::cout << it << " ";
    }
    std::cout << "\n";
    return 0;
}