#ifndef FLAGS_HPP_
#define FLAGS_HPP_

#include <iostream>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <string>
#include <vector>
#include <any>

template<typename>
struct is_vector : std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

struct Flags
{
    template<typename T>
    typename std::enable_if<!is_vector<T>::value, Flags*>::type
    with_arg(const std::string& name, char shortcut, T value, const std::string& desc);
    template<typename T>
    typename std::enable_if<is_vector<T>::value, Flags*>::type
    with_arg(const std::string& name, char shortcut, T value, const std::string& desc);
    Flags* with_opt(const std::string& name, char shortcut, const std::string& desc);
    Flags* with_help(const std::string& desc);
    void build();
    
    static Flags* parse(int argc, char** argv);
    template<typename T>
    typename std::enable_if<!is_vector<T>::value, T&>::type
    static arg(const std::string& key);
    template<typename T>
    typename std::enable_if<is_vector<T>::value, T>::type
    static arg(const std::string& key);
    template<typename T>
    static std::vector<T> args();
    static bool opt(const std::string& key);
private:
    std::unordered_map<std::string, std::any> data_;
    std::vector<std::string> args_;
    std::string app_name_;
    std::string help_desc_;
    char** argv_;
    int argc_;

    static Flags* instance();
    Flags() : argv_(nullptr), argc_(-1) {}
}; // struct Flags

template<typename T>
typename std::enable_if<!is_vector<T>::value, Flags*>::type
Flags::with_arg(const std::string& name, char shortcut, T value, const std::string& desc)
{
    std::cout << "arg\n";
    return this;
}

template<typename T>
typename std::enable_if<is_vector<T>::value, Flags*>::type
Flags::with_arg(const std::string& name, char shortcut, T value, const std::string& desc)
{
    std::cout << "vec arg\n";
    return this;
}

template<typename T>
typename std::enable_if<!is_vector<T>::value, T&>::type
Flags::arg(const std::string& key)
{
    std::cout << "arg\n";
    static T t;
    return t;
}

template<typename T>
typename std::enable_if<is_vector<T>::value, T>::type
Flags::arg(const std::string& key)
{
    std::cout << "args\n";
    T t;
    return t;
}

template<typename T>
std::vector<T> Flags::args()
{
    std::vector<T> result;
    return result;
}

#endif // FLAGS_HPP_


// ============= IMPLEMENTATION ============
#define FLAGS_IMPLEMENTATION


#ifdef FLAGS_IMPLEMENTATION

Flags* Flags::instance()
{
    static Flags *instance_{nullptr};
    static std::once_flag flag;
    if (!instance_) {
        std::call_once(flag, [&]() -> void {
            instance_ = new (std::nothrow) Flags();
        });
    }
    return instance_; 
}

Flags* Flags::parse(int argc, char** argv)
{
    auto ins{instance()};
    ins->app_name_ = argv[0];
    ins->argc_ = argc;
    ins->argv_ = argv;
    return ins;
}

Flags* Flags::with_opt(const std::string& name, char shortcut, const std::string& desc)
{
    return this;
}

Flags* Flags::with_help(const std::string& desc)
{
    help_desc_ = desc;
    return this;
}

void Flags::build()
{

}

bool Flags::opt(const std::string& key)
{
    return false;
}

#endif // FLAGS_IMPLEMENTATION
