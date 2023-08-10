#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "arguments.hpp"
#include <iomanip>
#include <map>
#ifdef COPY_FILES_C
#include "directory.h"
#endif

namespace fs = std::filesystem;

struct file_description_t
{
    std::uintmax_t size;
    fs::path path;
};

struct file_group_t
{
    std::vector<file_description_t> files;
    size_t disk_index;
    size_t group_size;
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
        i = rootPath / i;
        if (fs::exists(i) && fs::is_regular_file(i))
        {
            std::uintmax_t size = fs::file_size(i);
            files.push_back({size, i});
        }
        else
        {
            std::cout << "invalid file " << i << std::endl;
        }
    }

    return files;
}

std::map<std::string, file_group_t> group_files(const std::vector<file_description_t> &files)
{
    std::map<std::string, file_group_t> res;
    for (auto &file : files)
    {
        file_group_t new_group;
        new_group.files.push_back(file);
        new_group.group_size = file.size;
        res.emplace(file.path, new_group);
    }
    return res;
}

std::map<std::string, file_group_t> group_files(const std::vector<file_description_t> &files, fs::path root_folder, uint32_t depth)
{
    if (depth == 0)
    {
        return group_files(files);
    }
    std::map<std::string, file_group_t> res_map;
    for (auto &file : files)
    {
        uint32_t i = 0;
        fs::path group_path;

        for (auto &sub : fs::relative(file.path, root_folder))
        {
            group_path = group_path / sub;
            if (++i >= depth)
            {
                break;
            }
        }
        res_map.try_emplace(group_path.string(), file_group_t());
        file_group_t &container = res_map.at(group_path.string());
        container.files.push_back(file);
        container.group_size += file.size;
    }
    return res_map;
}

// Function to group files into bags based on the given maximum bag size
void assign_disks(std::map<std::string, file_group_t> &groups, std::uintmax_t max_bag_size, bool compact)
{
    size_t current_bag = 0;
    std::uintmax_t current_bag_size = 0;
    size_t n_dones = 0;
    file_description_t *file;

    // set all group indexes to zero
    for (auto &[key, group] : groups)
    {
        group.disk_index = -1;
    }

    while (n_dones < groups.size())
    {
        for (auto &[key, group] : groups)
        {
            if (group.disk_index !=-1)
            {
                // Already allocated
                continue;
            }

            if (group.group_size > max_bag_size)
            {
                std::cout << "group " << key << " is too large" << std::endl;
                exit(1);
            }

            if (current_bag_size + group.group_size <= max_bag_size)
            {
                group.disk_index = current_bag;
                current_bag_size += group.group_size;
                n_dones++;
            }
            else if (!compact)
            {
                break;
            }

            // Print progression
            float progression = ((float)n_dones / (float)groups.size()) * 100;
            std::cout << "\r" << "Progression: " << std::fixed << std::setprecision(2) << progression << "%" << std::flush;
        }


        current_bag++;
        current_bag_size = 0;
    }
    std::cout << std::endl;
}

typedef struct
{
    size_t n_elements;
    size_t total_size;
} disk_stat_t;

std::vector<disk_stat_t> disk_stats(const std::map<std::string, file_group_t> &groups)
{

    // find the min value of group.disk_index
    size_t min_disk_index = std::numeric_limits<size_t>::max();
    for (auto &[key, group] : groups)
    {
        if (group.disk_index != 0)
        {
            min_disk_index = std::min(min_disk_index, group.disk_index);
        }
    }

    // find the max value of group.disk_index
    size_t n_disks = 0;
    for (auto &[key, group] : groups)
    {
        if (group.disk_index != 0)
        {
            n_disks = std::max(n_disks, group.disk_index);
        }
    }
    n_disks++;

    std::vector<disk_stat_t> res(n_disks);

    // init
    for (auto &i : res)
    {
        i.total_size = 0;
        i.n_elements = 0;
    }

    for (auto &[key, group] : groups)
    {
        res[group.disk_index].total_size += group.group_size;
        res[group.disk_index].n_elements += group.files.size();
    }
    return res;
}

std::vector<fs::path> parent_paths(const fs::path &input)
{
    std::vector<fs::path> res;

    for (const auto &i : input)
    {
        if (res.size() == 0)
        {
            res.push_back(i);
        }
        else
        {
            res.push_back(res.back() / i);
        }
    }
    return res;
}

// Function to copy files to subfolders bag1, bag2, bag3, etc.
void copy_files_to_subfolders(const std::map<std::string, file_group_t> &groups, const fs::path &output_directory, const std::string &folder_prefix, fs::path rootFolder)
{
    auto disks = disk_stats(groups);
    std::vector<size_t> copied = std::vector<size_t>(disks.size(), 0);
    std::vector<uint8_t> percents = std::vector<uint8_t>(disks.size(), 0);

    fs::path to_create, subfolder, dst;

    // current time in seconds

    for (uint16_t active_disk_index = 0; active_disk_index < disks.size(); active_disk_index++)
    {
        auto start = std::chrono::system_clock::now();

        for (const auto &[key, group] : groups)
        {
            auto disk_index = group.disk_index;
            if (disk_index == active_disk_index)
            {
                dst = output_directory / (folder_prefix + std::to_string(disk_index+1)) / fs::relative(key, rootFolder);
                // std::cout << (rootFolder / key).string() << " -> " << dst.string() << std::endl;
#ifdef COPY_FILE_C
                copy_folder((rootFolder / key).c_str(), dst.c_str());
#else
                fs::create_directories(dst.parent_path());
                auto src = rootFolder / key;
                fs::copy(src, dst, fs::copy_options::recursive | fs::copy_options::update_existing);

                // Logging
                copied[disk_index] += group.group_size;
                uint8_t new_percent = (100 * copied[disk_index]) / disks[disk_index].total_size;
                if (new_percent != percents[disk_index])
                {
                    // Refresh display
                    percents[disk_index] = new_percent;

                    //Compute copy size in kb/sec
                    auto end = std::chrono::system_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    float speed = (float)copied[disk_index] / ((float)(elapsed.count()*1000));

                    std::cout << "\t\r" << std::flush;
                    std::cout << "Disk " << disk_index+1 << ": " << (int)(new_percent) << "% at " << speed << " kb/sec" << std::flush;
                }
            }

#endif
            }
            std::cout << std::endl;
        }
    }

    void print_groups(std::map<std::string, file_group_t> & groups)
    {
        for (const auto [key, group] : groups)
        {
            std::cout << key << " (" << group.files.size() << " elements " << group.group_size << " bytes, disk " << group.disk_index << ")" << std::endl;
        }
    }

    void print_disks(std::map<std::string, file_group_t> & groups)
    {
        std::cout << "-----------" << std::endl << "Disks:" << std::endl;
        auto disks = disk_stats(groups);
        size_t n_disks = disks.size();

        for (size_t disk_index = 0; disk_index < n_disks; disk_index++)
        {
            auto disk = disks[disk_index];
            std::cout << "disk " << disk_index+1 << ": " << disk.n_elements << " elements and " << disk.total_size << "bytes" << std::endl;
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

        std::cout << "Found " << files.size() << " files." << std::endl;

        std::vector<file_description_t> file_sizes = file_size(files, rootFolder);

        if (file_sizes.empty())
        {
            std::cout << "No valid files found.\n";
            return 0;
        }

        auto groups = group_files(file_sizes, rootFolder, args.max_depth);

        std::cout << "Found " << groups.size() << " groups." << std::endl;
        std::cout << "Assigning disks..." << std::endl;

        assign_disks(groups, args.size, args.compact);

        if (args.max_depth){
            print_groups(groups);
        }
        print_disks(groups);

        std::cout << std::endl;

        if (!args.mock){
            copy_files_to_subfolders(groups, args.output, args.folderPrefix, rootFolder);
            std::cout << "Files copied to subfolders successfully.\n";
        }

        return 0;
    }
