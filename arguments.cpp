#include "arguments.hpp"

Arguments parse_arguments(int argc, char* argv[]) {
    argparse::ArgumentParser program("disc_filler");

    program.add_argument("-s", "--size")
        .help("The maximal disk size in bytes (default: size of a Blu-ray: 25000000000)")
        .default_value(25000000000)
        .action([](const std::string& value) { return std::stoull(value); });

    program.add_argument("-p", "--folder-prefix")
        .help("The name of the created folders (default: \"disk\" so the folder will be disk1, disk2, etc)")
        .default_value("disk");

    program.add_argument("-o", "--output")
        .help("The path to an output directory where the folders will be created")
        .required();

    try {
        program.parse_args(argc, argv);

        Arguments args;
        args.max_disk_size = program.get<std::uintmax_t>("--size");
        args.folder_prefix = program.get<std::string>("--folder-prefix");
        args.output_directory = program.get<fs::path>("--output");

        return args;

    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cout << program;
        exit(1);
    }
}
