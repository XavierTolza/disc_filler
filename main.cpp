#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "arguments.hpp"

namespace fs = std::filesystem;

struct file_description_t
{
    std::uintmax_t size;
    fs::path path;
};

std::vector<fs::path> read_stdin(std::istream &input)
{
    std::string line;
    std::vector<fs::path> res;

    while (std::getline(input, line))
    {
        fs::path file_path = line;
        res.push_back(file_path);
    }

    return res;
}

std::vector<fs::path> read_file(const fs::path &path)
{
    std::vector<fs::path> files;

    std::ifstream inputFile(path);
    if (!inputFile)
    {
        std::cerr << "Error: Unable to open the input file: " << path << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(inputFile, line))
    {
        // Trim leading and trailing whitespaces from the line
        line.erase(line.find_last_not_of(" \t") + 1);
        line.erase(0, line.find_first_not_of(" \t"));

        if (!line.empty())
        {
            files.push_back(line);
        }
    }

    return files;
}

std::vector<fs::path> find_files(const std::filesystem::path &rootDir)
{
    std::vector<fs::path> res;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(rootDir))
    {
        if (std::filesystem::is_regular_file(entry))
        {
            res.push_back(entry.path());
        }
    }
    return res;
}

// Function to get file size and paths and store them in a vector
std::vector<file_description_t> file_size(std::vector<fs::path> &input, fs::path rootPath)
{
    std::vector<file_description_t> files;
    std::string line;

    for (fs::path &i : input)
    {
        i = rootPath/i;
        if (fs::exists(i) && fs::is_regular_file(i))
        {
            std::uintmax_t size = fs::file_size(i);
            files.push_back({size, i});
        }
        else{
            std::cout << "invalid file " << i << std::endl;
        }
    }

    return files;
}

// Function to group files into bags based on the given maximum bag size
std::vector<std::vector<file_description_t>> group_files(const std::vector<file_description_t> &files, std::uintmax_t max_bag_size)
{
    std::vector<std::vector<file_description_t>> bags;
    std::vector<file_description_t> current_bag;
    std::uintmax_t current_bag_size = 0;
    size_t n_dones = 0;

    while (n_dones < files.size())
    {
        for (const auto &file : files)
        {
            if (file.size > max_bag_size)
            {
                std::cout << "file " << file.path << " is too large" << std::endl;
                exit(1);
            }

            if (current_bag_size + file.size <= max_bag_size)
            {
                current_bag.push_back(file);
                current_bag_size += file.size;
                n_dones++;
            }
        }

        bags.push_back(current_bag);
        current_bag.clear();
        current_bag_size = 0;
    }

    return bags;
}

// Function to copy files to subfolders bag1, bag2, bag3, etc.
void copy_files_to_subfolders(const std::vector<std::vector<file_description_t>> &bags, const fs::path &output_directory, const std::string &folder_prefix, fs::path rootFolder)
{
    for (size_t i = 0; i < bags.size(); ++i)
    {
        fs::path subfolder = output_directory / (folder_prefix + std::to_string(i + 1));
        for (const auto &file : bags[i])
        {
            auto dst = subfolder / fs::relative(file.path, rootFolder);
            std::cout << (file.path).string() << " -> " << dst.string() << std::endl;
            fs::create_directories(dst.parent_path());
            fs::copy(file.path, dst);
        }
    }
}

int main(int argc, char *argv[])
{
    Arguments args = parseCommandLine(argc, argv);

    if (!fs::exists(args.output))
    {
        std::cerr << "Error: Output directory does not exist.\n";
        return 1;
    }

    std::vector<fs::path> files;
    fs::path rootFolder = fs::current_path();

    if (args.input == "-")
    {
        // Read from stdin
        files = read_stdin(std::cin);
    }
    else if (fs::exists(args.input) && fs::is_regular_file(args.input))
    {
        files = read_file(args.input);
        rootFolder = fs::path(args.input).parent_path();
    }
    else if (fs::exists(args.output) && !fs::is_regular_file(args.input))
    {
        files = find_files(args.input);
        rootFolder = args.input;
    }
    else
    {
        std::cout << "invalid input file: " << args.input << std::endl;
        displayHelp();
        exit(1);
    }

    std::vector<file_description_t> file_sizes = file_size(files,rootFolder);

    if (file_sizes.empty())
    {
        std::cout << "No valid files found.\n";
        return 0;
    }

    std::vector<std::vector<file_description_t>> bags = group_files(file_sizes, args.size);

    if (bags.empty())
    {
        std::cout << "No files to copy.\n";
        return 0;
    }

    copy_files_to_subfolders(bags, args.output, args.folderPrefix, rootFolder);
    std::cout << "Files copied to subfolders successfully.\n";

    return 0;
}
