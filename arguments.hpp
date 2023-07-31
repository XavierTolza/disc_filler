#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <string>

struct Arguments {
    std::string input;
    long long size;
    std::string folderPrefix;
    std::string output;
    bool displayHelp;
};

void displayHelp();
Arguments parseCommandLine(int argc, char* argv[]);

#endif // ARGUMENTS_HPP
