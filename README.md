# Disc Filler

Disc Filler is a C++ program designed to help you group and organize files that you want to archive onto DVDs or Blu-ray discs. The program takes a list of files as input and groups them into folders (bags) of a user-defined maximal size. The goal is to ensure that each folder's total size does not exceed the capacity of the target disc, making it easier to burn the files onto multiple discs for archiving purposes.

## Table of Contents
- [Disc Filler](#disc-filler)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Features](#features)
  - [Requirements](#requirements)
  - [Usage](#usage)
  - [How It Works](#how-it-works)
  - [Building](#building)
  - [Contributing](#contributing)
  - [License](#license)

## Introduction

Archiving large amounts of data onto DVDs or Blu-ray discs can be a cumbersome task. Disc Filler simplifies this process by automatically organizing files into folders (bags) that fit the disc's capacity. This not only makes the data transfer more efficient but also ensures you utilize the disc space optimally.

## Features

- Group files into folders (bags) based on user-defined maximal size.
- Utilizes the C++ standard library's `<filesystem>` to handle file operations.
- Supports both DVDs and Blu-ray discs.
- Provides a straightforward command-line interface for easy interaction.
- Copies files to subfolders with appropriate naming for easy identification.

## Requirements

- C++ compiler with C++17 support.
- CMake (minimum version 3.15).

## Usage

1. Clone the repository to your local machine or download the source code as a ZIP file and extract it.
2. Make sure you have the required dependencies (C++ compiler, CMake) installed on your system.
3. Compile the program using CMake and your C++ compiler (See [Building](#building)).
4. Run the compiled executable "disc_filler."
5. Follow the on-screen instructions:
   - Enter the maximum bag size (the size that each folder can hold, measured in bytes).
   - Input the list of files to be archived (one file path per line).
   - The program will automatically group the files into bags based on the specified size and copy them to appropriately named subfolders.

## How It Works

1. The program takes the list of input files and their sizes and stores them in a vector of `file_description_t` structures, which contain the file size in bytes and the file path.
2. It then groups the files into bags (subfolders) based on the user-defined maximal bag size. Each bag's size will not exceed the specified maximum.
3. Files are iteratively added to each bag until the bag's size is close to exceeding the user-defined maximum. Once that happens, a new bag is created, and the process continues until all files are allocated to their respective bags.
4. Finally, the program copies the files to subfolders ("bag1", "bag2", etc.) based on their groupings. These subfolders are ready to be burned onto DVDs or Blu-ray discs for archiving.

## Building

To build the Disc Filler program, follow these steps:

1. Create a "build" directory in the root of the project.
2. Navigate to the "build" directory in your terminal or command prompt.
3. Run CMake to configure the build:
   ```bash
   cmake ..
   ```
4. Build the executable:
   ```bash
   cmake --build .
   ```
5. The compiled executable named "disc_filler" will be available in the "build" directory.

## Contributing

Contributions to Disc Filler are welcome! If you find any bugs, have feature requests, or want to improve the program, please feel free to submit an issue or a pull request on the GitHub repository.

## License

Disc Filler is released under the [MIT License](LICENSE). You are free to use, modify, and distribute the code as per the terms of the license. However, please note that the program comes with no warranty or guarantee, and the author shall not be held liable for any damages arising from its use. It is recommended to review the license file before using the software.