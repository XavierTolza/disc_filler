# Project Name: disc_filler

The `disc_filler` project is a C++ program designed to group files for archiving on DVD/Blu-ray discs or any other storage media into folders of a maximum size. It offers a command-line interface that allows users to specify the input files, the size limit for each folder, and other configuration options. The program intelligently groups the files based on their size and ensures that each folder does not exceed the specified size limit.

## Features

- Groups files into folders based on a specified maximum folder size.
- Supports reading file paths from stdin or from a file.
- Recursively searches for files in the input directory and its subdirectories.
- Allows limiting the depth of directory hierarchy when creating groups.
- Provides options for compact grouping or not (when not compact, it might create fewer groups).
- Displays information about the generated groups and their sizes before archiving.
- Copies the files to subfolders with a given prefix and a numbered index (e.g., bag1, bag2, ...).
- Supports copying files using standard C++ filesystem library or using a custom `copy_folder` function (controlled by preprocessor macros).
- Provides a help message to guide users in using the command-line arguments.

## Usage

### Compilation

To compile the `disc_filler` project, use CMake with the following steps:

1. Create a `CMakeLists.txt` file with the appropriate settings (as provided in the earlier conversation).
2. Run the CMake command to generate the build files for your specific platform.
3. Build the project using the generated build files (e.g., `make` on Unix-based systems or `msbuild` on Windows).

### Running the Program

The `disc_filler` program is a command-line tool that accepts various command-line arguments. Here is the general syntax:

```
disc_filler [OPTIONS]
```

#### Command-line Options

- `-i`, `--input`: Specifies the input file or directory from which the program reads the files to group. Use "-" to read file paths from stdin.
- `-s`, `--size`: Sets the maximum size limit (in bytes) for each group folder.
- `-p`, `--prefix`: Specifies the prefix to use for the group folder names. Each folder will be named with the prefix followed by a numeric index (e.g., bag1, bag2, ...).
- `-o`, `--output`: Specifies the output directory where the grouped files will be copied.
- `--max-depth`: Limits the depth of directory hierarchy when creating groups. Files in subdirectories beyond this depth will not be considered for grouping.
- `--compact`: Uses compact grouping (fewer groups) when specified.
- `-h`, `--help`: Displays the help message, explaining the usage and options.

**Note**: If you use the `--compact` option, the program might create fewer groups, and the total size of each group might approach the specified maximum size more closely.

### Example Usage

```bash
# Group files in "input_directory" into folders with a maximum size of 1GB in the "output_directory"
$ ./disc_filler -i input_directory -s 1073741824 -o output_directory

# Read file paths from "input_files.txt" and group them into folders with a maximum size of 700MB in "output_directory"
$ ./disc_filler -i input_files.txt -s 734003200 -o output_directory

# Read file paths from stdin and group them compactly into folders with a maximum size of 500MB in "output_directory"
$ cat input_files.txt | ./disc_filler -i - -s 524288000 -o output_directory --compact
```

### Compilation Options

The project offers a few preprocessor macros that control how the files are copied. By default, it uses the standard C++ filesystem library for copying files. However, if you want to use a custom `copy_folder` function (e.g., `copy_folder` provided in `directory.h`), you can define the macro `COPY_FILES_C` before compiling the program.

## License

This project is licensed under the [MIT License](LICENSE).

## Contribution

Contributions to the `disc_filler` project are welcome! If you find any issues, have suggestions for improvements, or want to add new features, please open a GitHub issue or submit a pull request.

## Acknowledgments

The `disc_filler` project was created by [Your Name] and was inspired by the need to efficiently archive files on optical media. It uses the standard C++ library for file operations and supports flexible configuration to suit various archiving scenarios. The project is built upon the C++ programming language, CMake build system, and the power of the filesystem library to efficiently handle file operations.