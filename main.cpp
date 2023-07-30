#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "arguments.hpp"

namespace fs = std::filesystem;

struct file_description_t {
    std::uintmax_t size;
    fs::path path;
};

// Function to get file size and paths and store them in a vector
std::vector<file_description_t> file_size(std::istream& input) {
    std::vector<file_description_t> files;
    std::string line;

    while (std::getline(input, line)) {
        fs::path file_path(line);
        if (fs::exists(file_path) && fs::is_regular_file(file_path)) {
            std::uintmax_t size = fs::file_size(file_path);
            files.push_back({size, file_path});
        }
    }

    return files;
}

// Function to group files into bags based on the given maximum bag size
std::vector<std::vector<file_description_t>> group_files(const std::vector<file_description_t>& files, std::uintmax_t max_bag_size) {
    std::vector<std::vector<file_description_t>> bags;
    std::vector<file_description_t> current_bag;
    std::uintmax_t current_bag_size = 0;

    for (const auto& file : files) {
        if (current_bag_size + file.size <= max_bag_size) {
            current_bag.push_back(file);
            current_bag_size += file.size;
        } else {
            bags.push_back(current_bag);
            current_bag.clear();
            current_bag.push_back(file);
            current_bag_size = file.size;
        }
    }

    if (!current_bag.empty()) {
        bags.push_back(current_bag);
    }

    return bags;
}

// Function to copy files to subfolders bag1, bag2, bag3, etc.
void copy_files_to_subfolders(const std::vector<std::vector<file_description_t>>& bags, const fs::path& output_directory, const std::string& folder_prefix) {
    for (size_t i = 0; i < bags.size(); ++i) {
        fs::path subfolder = output_directory / (folder_prefix + std::to_string(i + 1));
        fs::create_directory(subfolder);
        for (const auto& file : bags[i]) {
            fs::copy(file.path, subfolder / file.path.filename());
        }
    }
}

int main(int argc, char* argv[]) {
    Arguments args = parse_arguments(argc, argv);

    if (!fs::exists(args.output_directory)) {
        std::cerr << "Error: Output directory does not exist.\n";
        return 1;
    }

    // std::cout << "Enter the list of files (one per line, end with Ctrl+Z on Windows or Ctrl+D on Linux/macOS):\n";
    std::vector<file_description_t> files = file_size(std::cin);

    if (files.empty()) {
        std::cout << "No valid files found.\n";
        return 0;
    }

    std::vector<std::vector<file_description_t>> bags = group_files(files, args.max_disk_size);

    if (bags.empty()) {
        std::cout << "No files to copy.\n";
        return 0;
    }

    copy_files_to_subfolders(bags, args.output_directory, args.folder_prefix);
    std::cout << "Files copied to subfolders successfully.\n";

    return 0;
}
