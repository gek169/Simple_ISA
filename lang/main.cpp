#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <map>

int main(int argc, char *argv[])
{
    std::vector<std::string_view> args;
    {
        std::vector<char*> args2(argv, argv + argc);
        args.reserve(args2.size());
        for(auto arg : args2)
            args.push_back(std::string_view(arg));
    }

    static const char *EMPTY = "";

    std::vector<std::string_view> values;
    std::map<std::string_view, std::string_view> options; 

    {
        std::string_view current = EMPTY;
        for(auto arg : args)
        {
            if(arg.size() == 0) continue;
            if(arg[0] == '-') // option.
            {
                /**/ if(arg.size() == 1) break; // for now.
                else if(arg[1] == '-')
                {
                    if(arg.size() > 2)
                    {
                        options[current = std::string_view(arg.begin() + 2, arg.size() - 2)] = EMPTY;
                    }
                    else break; // for now.
                }
                else if(arg.size() > 1)
                {
                    options[current = std::string_view(arg.begin() + 1, arg.size() - 1)] = EMPTY;
                }
            }
            else
            {
                if(current == EMPTY) values.push_back(arg);
                else
                {
                    options[current] = arg;
                    current = EMPTY;
                }
            }
        }
    }

    for(const auto &value : values)
    {
        std::cout << "Value: '" << (value.data() == EMPTY ? "<empty>" : value) << "'\n";
    }
    std::cout << std::endl;

    for(const auto &opt : options)
    {
        std::cout
            << "Option: '"
            << (opt.first.data() == EMPTY ? "<empty>" : opt.first)
            << "' = '"
            << (opt.second.data() == EMPTY ? "<empty>" : opt.second)
            << "'\n";
    }
    std::cout << std::endl;

    return 0;
}
