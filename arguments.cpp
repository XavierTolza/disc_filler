#include "arguments.hpp"
#include <iostream>
#include <unistd.h>
#include <getopt.h>

void displayHelp() {
    std::cout << "Usage: ./your_program [OPTIONS]\n"
              << "Options:\n"
              << "  -i, --input <path>   Input file path or folder to read file names from, or \"-\" to read from stdin\n"
              << "  -s, --size <bytes>   Maximal disk size in bytes (default: 25000000000)\n"
              << "  -p, --folder-prefix <name>   Name of the created folders (default: \"disk\")\n"
              << "  -o, --output <path>  Path to the output directory where the folders will be created\n"
              << "  -h, --help           Display this help text\n";
}

Arguments parseCommandLine(int argc, char* argv[]) {
    Arguments args;
    args.size = 25000000000; // Default size of a Blu-ray disc in bytes
    args.folderPrefix = "disk"; // Default folder prefix
    args.displayHelp = false;

    const char* shortOpts = "i:s:p:o:h";
    const option longOpts[] = {
        {"input", required_argument, nullptr, 'i'},
        {"size", required_argument, nullptr, 's'},
        {"folder-prefix", required_argument, nullptr, 'p'},
        {"output", required_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1) {
        switch (opt) {
            case 'i':
                args.input = optarg;
                break;
            case 's':
                args.size = std::stoll(optarg);
                break;
            case 'p':
                args.folderPrefix = optarg;
                break;
            case 'o':
                args.output = optarg;
                break;
            case 'h':
                args.displayHelp = true;
                break;
            default:
                std::cerr << "Invalid option. Use -h for help.\n";
                args.displayHelp = true;
                return args;
        }
    }

    return args;
}
