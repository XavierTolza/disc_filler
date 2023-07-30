#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct Arguments {
    std::uintmax_t max_disk_size;
    std::string folder_prefix;
    fs::path output_directory;
};

Arguments parse_arguments(int argc, char* argv[]);

#endif // ARGUMENTS_HPP
