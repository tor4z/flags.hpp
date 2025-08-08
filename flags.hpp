#ifndef FLAGS_HPP_
#define FLAGS_HPP_

#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <any>

#define GLAGS_UNKNOWN_OPTION(NAME)                  \
    do {                                            \
        std::cerr << "Bad option: " << NAME;        \
        assert(false && "Unknown option");          \
    } while (0)

#define GLAGS_DUPLICATED_OPTION(NAME)               \
    do {                                            \
        std::cerr << "Refine optino: " << NAME;     \
        assert(false && "Bad option");              \
    } while (0)

#define GLAGS_CHECK_OPTION_DUPLICATED(NAME)         \
    do {                                            \
        if (data_.find(NAME) != data_.end()) {      \
            GLAGS_DUPLICATED_OPTION(NAME);          \
        }                                           \
    } while (0)

template<typename>
struct is_vector : std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template<typename T>
T flags_cast_string(const std::string& v)
{
    assert(false && "Not supported string cast");
}

template<>
inline int flags_cast_string<int>(const std::string& v)
{
    return std::stoi(v);
}

template<>
inline float flags_cast_string<float>(const std::string& v)
{
    return std::stof(v);
}

template<>
inline double flags_cast_string<double>(const std::string& v)
{
    return std::stod(v);
}

template<>
inline long flags_cast_string<long>(const std::string& v)
{
    return std::stol(v);
}

template<>
inline long long flags_cast_string<long long>(const std::string& v)
{
    return std::stoll(v);
}

template<>
inline unsigned long flags_cast_string<unsigned long>(const std::string& v)
{
    return std::stoul(v);
}

template<>
inline unsigned long long flags_cast_string<unsigned long long>(const std::string& v)
{
    return std::stoull(v);
}

struct Flags
{
    template<typename T>
    Flags* with_arg(const std::string& name, char shortcut, T value, const std::string& desc);
    template<typename T>
    Flags* with_arg(const std::string& name, char shortcut, const std::vector<T>& value, const std::string& desc);
    Flags* with_opt(const std::string& name, char shortcut, const std::string& desc);
    void set_help(const std::string& desc);

    static Flags* parse(int argc, char** argv);
    template<typename T>
    typename std::enable_if<!is_vector<T>::value, T>::type
    static arg(const std::string& key);
    template<typename T>
    typename std::enable_if<is_vector<T>::value, T>::type
    static arg(const std::string& key);
    template<typename T>
    static std::vector<T> args();
    static bool opt(const std::string& key);
private:
    template<typename T>
    struct Arg {
        std::string long_name;
        T value;
        bool has_extra_arg;
        char shor_name;
    }; // struct Arg

    std::unordered_map<std::string, std::any> data_;
    std::vector<std::string> args_;
    std::string app_name_;
    std::stringstream help_doc_ss_;
    std::vector<std::string> input_args_;
    bool exit_with_help_;

    static Flags* instance();
    Flags() : exit_with_help_(false) {}
    void show_help(const std::string& desc);
    bool is_short_option(const std::string& arg);
    bool is_long_option(const std::string& arg);
}; // struct Flags

template<typename T>
Flags* Flags::with_arg(const std::string& name, char shortcut, T value, const std::string& desc)
{
    GLAGS_CHECK_OPTION_DUPLICATED(name);

    help_doc_ss_ << "\n  -" << shortcut << ", --" << name << " <" << value << ">" << "\t\t"<< desc;
    Arg<T> arg;
    arg.long_name = name;
    arg.shor_name = shortcut;
    arg.value = value;
    arg.has_extra_arg = true;

    bool expect_param{false};
    for (const auto& it : input_args_) {
        if (is_long_option(it)) {
            if (expect_param) {
                exit_with_help_ = true;
                return this;
            }

            if (name == it.substr(2)) {
                expect_param = true;
            }
        } else if (is_short_option(it)) {
            if (expect_param) {
                exit_with_help_ = true;
                return this;
            }

            for (char c: it) {
                if (expect_param) {
                    exit_with_help_ = true;
                    return this;
                }

                if (c == shortcut) {
                    expect_param = true;
                }
            }
        } else {
            if (expect_param) {
                arg.value = flags_cast_string<T>(it);
                expect_param = false;
                break;
            }
        }
    }

    if (expect_param) {
        exit_with_help_ = true;
        return this;
    }

    data_.insert(std::make_pair(name, arg));
    return this;
}

template<typename T>
Flags* Flags::with_arg(const std::string& name, char shortcut, const std::vector<T>& value, const std::string& desc)
{
    GLAGS_CHECK_OPTION_DUPLICATED(name);

    help_doc_ss_ << "\n  -" << shortcut << ", --" << name << " <";
    for (size_t i = 0; i < value.size(); ++i) {
        help_doc_ss_ << value.at(i);
        if (i < value.size() - 1) help_doc_ss_ << ", ";
    }    
    help_doc_ss_ << ">\t\t"<< desc;

    Arg<std::vector<T>> arg;
    arg.long_name = name;
    arg.shor_name = shortcut;
    arg.has_extra_arg = true;

    bool use_default{true};
    bool expect_param{false};
    for (const auto& it : input_args_) {
        if (is_long_option(it)) {
            if (expect_param) {
                if (arg.value.empty()) {
                    exit_with_help_ = true;
                    return this;
                }
                break;
            }

            if (name == it.substr(2)) {
                expect_param = true;
            }
        } else if (is_short_option(it)) {
            if (expect_param) {
                if (arg.value.empty()) {
                    exit_with_help_ = true;
                    return this;
                }
                break;
            }

            for (char c: it) {
                if (expect_param) {
                    if (arg.value.empty()) {
                        exit_with_help_ = true;
                        return this;
                    }
                    break;
                }

                if (c == shortcut) {
                    expect_param = true;
                }
            }
        } else {
            if (expect_param) {
                arg.value.push_back(flags_cast_string<T>(it));
                use_default = false;
            }
        }
    }

    if (expect_param && arg.value.empty()) {
        exit_with_help_ = true;
        return this;
    }

    if (use_default) {
        arg.value = value;
    }

    data_.insert(std::make_pair(name, arg));
    return this;
}

template<typename T>
typename std::enable_if<!is_vector<T>::value, T>::type
Flags::arg(const std::string& key)
{
    auto ins{instance()};
    auto find_result{ins->data_.find(key)};
    if (find_result == ins->data_.end()) {
        GLAGS_UNKNOWN_OPTION(key);
    }

    return std::any_cast<Arg<T>>(find_result->second).value;
}

template<typename T>
typename std::enable_if<is_vector<T>::value, T>::type
Flags::arg(const std::string& key)
{
    auto ins{instance()};
    auto find_result{ins->data_.find(key)};
    if (find_result == ins->data_.end()) {
        GLAGS_UNKNOWN_OPTION(key);
    }

    return std::any_cast<Arg<T>>(find_result->second).value;
}

template<typename T>
std::vector<T> Flags::args()
{
    auto ins{instance()};
    std::vector<T> result;
    for (const auto& it : ins->args_) {
        result.push_back(flags_cast_string<T>(it));
    }
    return result;
}

#endif // FLAGS_HPP_


// ============= IMPLEMENTATION ============
#define FLAGS_IMPLEMENTATION // delete me


#ifdef FLAGS_IMPLEMENTATION


void Flags::show_help(const std::string& desc)
{
    std::cout << app_name_ << ": "
    << desc << "\nOptions:"
    << help_doc_ss_.str() << "\n\n";
    exit(0);
}

void Flags::set_help(const std::string& desc)
{
    const std::string name{"help"};
    const char shortcut{'h'};

    for (const auto& it : input_args_) {
        if (is_long_option(it)) {
            if (name == it.substr(2)) {
                exit_with_help_ = true;
                break;
            }
        } else if (is_short_option(it)) {
            for (char c: it) {
                if (c == shortcut) {
                    exit_with_help_ = true;
                    break;
                }
            }
        }
    }

    if (exit_with_help_) {
        show_help(desc);
    }
}

bool Flags::is_short_option(const std::string& arg)
{
    if (arg.size() >= 2) {
        if (arg[0] != '-') {
            return false;
        }

        if (arg[1] != '-') {
            return true;
        }
    }
    return false;
}

bool Flags::is_long_option(const std::string& arg)
{
    if (arg.size() >= 3) {
        if (arg[0] == '-' && arg[1] == '-') {
            if (arg[2] == '-') {
                return false;
            }
            return true;
        }
    }
    return false;
}

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
    for (int i = 1; i < argc; ++i) {
        ins->input_args_.push_back(argv[i]);
    }
    return ins;
}

Flags* Flags::with_opt(const std::string& name, char shortcut, const std::string& desc)
{
    GLAGS_CHECK_OPTION_DUPLICATED(name);

    help_doc_ss_ << "\n  -" << shortcut << ", --" << name << "\t\t"<< desc;
    Arg<bool> arg;
    arg.long_name = name;
    arg.shor_name = shortcut;
    arg.has_extra_arg = false;
    arg.value = false;

    for (const auto& it: input_args_) {
        if (is_long_option(it)) {
            if (name == it.substr(2)) {
                arg.value = true;
                break;
            }
        } else if (is_short_option(it)) {
            for (char c: it) {
                if (c == shortcut) {
                    arg.value = true;
                    break;
                }
            }
        }
    }

    data_.insert(std::make_pair(name, arg));

    return this;
}

bool Flags::opt(const std::string& key)
{
    auto ins{instance()};
    auto find_result{ins->data_.find(key)};
    if (find_result == ins->data_.end()) {
        GLAGS_UNKNOWN_OPTION(key);
    }

    return std::any_cast<Arg<bool>>(find_result->second).value;
}

#endif // FLAGS_IMPLEMENTATION
