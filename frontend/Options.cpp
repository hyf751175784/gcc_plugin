#include "Options.h"

void Options::Trim(std::string &inout_s)
{
    int index = 0;
    if (!inout_s.empty())
    {
        while ((index = inout_s.find(' ', index)) != std::string::npos)
        {
            inout_s.erase(index, 1);
        }
    }
}

Options::Options(const std::vector<std::string> &lines, const std::vector<std::string> &options)
{
    num = 0;
    for (auto &line : lines)
    {
        _usage_lines.push_back(line);
    }
    for (auto &word : options)
    {
        _dict[word] = true;
        ++num;
    }
}

void Options::exec_file(std::string filename)
{
    _option_index_by_names.clear();
    std::ifstream in(filename.c_str());
    if (!in)
    {
        std::cout << "File not found\n";
        exit(1);
    }
    while (in)
    {
        // Read an entire line at a time
        std::string line;
        std::getline(in, line);
        Trim(line);

        std::regex e("([a-zA-Z]+)=([_a-zA-Z0-9.]+)");
        std::smatch matches;

        // 使用 std::regex_search() 函数在字符串中查找匹配项
        if (std::regex_search(line, matches, e))
        {
            if (_option_index_by_names.find(matches[1]) == _option_index_by_names.end())
            {
                if (_dict.find(matches[1]) != _dict.end())
                {
                    _option_index_by_names[matches[1]] = matches[2];
                }
                else
                {
                    std::cout << "Invalid option entry '" << matches[1] << "'\n";
                    exit(1);
                }
            }
            else
            {
                std::cout << "Duplicated parameter names\n";
                exit(1);
            }
        }
    }
}

void Options::parse(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string argvi = std::string(argv[i]);
        // std::cout << argvi << "\n";
        std::regex e("([a-zA-Z]+)=([_a-zA-Z0-9.]+)");
        std::smatch matches;

        // 使用 std::regex_search() 函数在字符串中查找匹配项
        if (std::regex_search(argvi, matches, e))
        {
            if (_option_index_by_names.find(matches[1]) == _option_index_by_names.end())
            {
                if (_dict.find(matches[1]) != _dict.end())
                {
                    _option_index_by_names[matches[1]] = matches[2];
                }
                else
                {
                    std::cout << "Invalid option entry '" << matches[1] << "'\n";
                    exit(1);
                }
            }
            else
            {
                std::cout << "Duplicated parameter names\n";
                exit(1);
            }
        }
        else
        {
            show_usage();
        }
    }
}

// 此函数调用命令行，负责和后端进行交互，先不实现
void Options::exec_cmd(std::map<std::string, std::string> &mmp)
{
    for (auto &kv : mmp)
    {
        std::cout << "exec_cmd: " << kv.first << ", " << kv.second << std::endl;
    }
};

void Options::exec_param()
{
    // 首先寻找有没有config参数 ，如果有，执行exec_file
    std::map<std::string, std::string>::iterator iter = _option_index_by_names.find("config");
    if (iter != _option_index_by_names.end())
    {
        exec_file(iter->second);
        exec_cmd(_option_index_by_names);
    }
    // 如果没有 则调用命令行
    else
    {
        exec_cmd(_option_index_by_names);
    }
}

void Options::show_usage()
{
    for (auto &line : _usage_lines)
    {
        std::cout << line << std::endl;
    }
    exit(1);
}