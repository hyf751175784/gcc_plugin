
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <fstream>

class Options
{
private:
    std::map<std::string, std::string> _option_index_by_names;
    std::vector<std::string> _usage_lines;
    std::map<std::string, bool> _dict;
    int num;

    void Trim(std::string &inout_s);

public:
    Options(const std::vector<std::string> &lines, const std::vector<std::string> &options);

    void exec_file(std::string filename);

    void parse(int argc, char *argv[]);

    // 此函数调用命令行，负责和后端进行交互，先不实现
    void exec_cmd(std::map<std::string, std::string> &mmp);

    void exec_param();

    void show_usage();
};