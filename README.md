# flags.hpp: A header only c++ module to parse command line arguments


### Usage

```c++
#define FLAGS_IMPLEMENTATION // stb style: https://github.com/nothings/stb
#include "flags.hpp"

// Setting command line parser 
Flags::parse(argc, argv)
    ->with_arg("test", 't', 100, "int test value")
    ->with_arg("args", 'a', std::vector<int>{1, 3, 4}, "some arg list")
    ->with_opt("option", 'o', "option test")
    ->set_help("basic example");

// Use arguments and options
std::cout << "test: " << Flags::arg<int>("test") << "\n";
std::cout << "option: " << Flags::opt("option") << "\n";

// Iterate program paramters
for (const auto& it : Flags::raw_params()) {
    std::cout << it << " ";
}
```

More examples please move to `examples/`


### License

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org>
