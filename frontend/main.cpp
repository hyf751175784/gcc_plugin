#include <iostream>
#include <string>
#include <vector>

#include "Options.h"

std::vector<std::string> usage = {
    "Usage: program_name [options...]",
    "",
    "Options required:",
    "  mutate=<filetotest>",
    "  build=<make>",
    "",
};

std::vector<std::string> ops = {
    "mutate",
    "build",
    "config",
    "op",
    "from",
    "to",
};

static Options options(usage, ops);

int main(int argc, char *argv[])
{
    /* g++ -std=c++11 main.cpp -o*/
    /* ./main -mutate=1 -build=2 */
    options.parse(argc, argv);
    options.exec_param();
    return 0;
}