#include <vector>
#define FLAGS_IMPLEMENTATION
#include "flags.hpp"
#include <iostream>


int main(int argc, char** argv)
{
    Flags::parse(argc, argv)
        ->with_arg("test", 't', 100, "int test value")
        ->with_arg("args", 'a', std::vector<int>{1, 3, 4}, "some arg list")
        ->with_opt("option", 'o', "option test")
        ->set_help("basic example");

    std::cout << "test: " << Flags::arg<int>("test") << "\n";
    std::cout << "option: " << Flags::opt("option") << "\n";
    std::cout << "extra args empty: " << Flags::args<std::string>().empty() << "\n";

    std::cout << "args: ";
    auto args{Flags::arg<std::vector<int>>("args")};
    for (auto it : args) {
        std::cout << it << " ";
    }
    std::cout << "\n";
    return 0;
}
