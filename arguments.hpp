#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <string>
#include <stdint.h>

struct Arguments
{
    std::string input;
    long long size;
    std::string folderPrefix;
    std::string output;
    bool displayHelp;
    uint32_t max_depth;
};

void displayHelp();
Arguments parseCommandLine(int argc, char *argv[]);

#endif // ARGUMENTS_HPP
