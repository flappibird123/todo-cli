#include "parser.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <format>
#include <vector>
#include <filesystem>
#include <system_error>
#include <fstream>
#include <variant>
#include <cctype>
#include <cstdlib>

#include "runner.hpp"

namespace fs = std::filesystem;

// TODO: Implement use of fileNum to make sure each file is evenly split at around 1000 lines of items per text file for efficient lookup
namespace {
    fs::path getStateDirectory() {
        if (const char* xdg = std::getenv("XDG_STATE_HOME")) {
            return std::filesystem::path(xdg);
        }

        if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / ".local" / "state";
        }

        throw std::runtime_error("Cannot determine home directory");
    }

    void ensureTodoDirectory() {
        std::error_code ec;
        std::filesystem::create_directories(getStateDirectory(), ec);
        if (ec) {
            throw std::runtime_error("Failed to create state directory: " + ec.message());
        }
    }

    void dirInit() {
        ensureTodoDirectory(); // make sure ~/.local/state exists

        auto todo_dir = getStateDirectory() / "todo";

        std::error_code ec;
        std::filesystem::create_directories(todo_dir, ec);
        if (ec) {
            throw std::runtime_error("Failed to create todo directory: " + ec.message());
        }
    }

    bool dataExists() {
        fs::path filePath = fs::path(getenv("HOME")) / ".local/state/todo/data.txt";
        return fs::exists(filePath) && fs::is_regular_file(filePath);
    }

    void createDataFile() {
        fs::path dirPath;

        #ifdef _WIN32
            // On Windows, use %LOCALAPPDATA% or %USERPROFILE%
            const char* localAppData = std::getenv("LOCALAPPDATA");
            if (!localAppData) {
                throw std::runtime_error("LOCALAPPDATA not set.\n");
            }
            dirPath = fs::path(localAppData) / "todo";
        #else
            // On Linux/macOS, use $HOME
            const char* homeDir = std::getenv("HOME");
            if (!homeDir) {
                throw std::runtime_error("HOME environment variable not set.\n");
            }
            dirPath = fs::path(homeDir) / ".local" / "state" / "todo";
        #endif
        fs::path filePath = dirPath / "data.txt";
        std::ofstream file(filePath);
        if (!file) {
            throw std::runtime_error("Failed to create file\n");
        }
    }

    std::variant<std::ifstream, std::ofstream> openDataFile(char mode) {
        const char* home = std::getenv("HOME");
        if (!home) {
            throw std::runtime_error("HOME environment variable not set!");
        }

        fs::path path = fs::path(home) / ".local" / "state" / "todo" / "data.txt";

        if (mode == 'r') {
            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file for reading: " + path.string() + '\n');
            }
            return file;
        } 
        else if (mode == 'w') {
            std::ofstream file(path, std::ios::app);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file for writing: " + path.string() + '\n');
            }
            return file;
        } 
        else {
            throw std::invalid_argument("Invalid mode. Use 'r' for reading or 'w' for writing.\n");
        }
    }

    std::string getFileNum() {
        dirInit();

        if (dataExists()) {
            try {
                auto file = openDataFile('r');
                if (std::holds_alternative<std::ifstream>(file)) {
                    std::ifstream& inFile = std::get<std::ifstream>(file);
                    std::string line;
                    if (std::getline(inFile, line)) {
                        do {
                            if (line.starts_with("<todo-file>")) {
                                std::string fnumber;
                                int i = 11;
                                while (std::isdigit(line.at(i))) {
                                    fnumber.push_back(line.at(i));
                                    ++i;
                                }
                                return fnumber;
                            }
                        } while (std::getline(inFile, line));
                    } else {
                        inFile.close();
                        auto oFile = openDataFile('w');
                        std::ofstream& outFile = std::get<std::ofstream>(oFile);
                        outFile << "<todo-file>1</todo-file>\n";
                        return "1";
                    }
                } 
                

            } catch (std::exception &e) {
                throw std::runtime_error(std::string(e.what()) + '\n');
            }
        } else {
            createDataFile();
            return getFileNum();
        }
        throw std::runtime_error("Unable to get file number\n");
    }
}

void parse(int argc, const char *argv[]) {
    if (argc == 1) {
        std::string msg = std::format("Usage: {} <command> [options]\n", argv[0]);
        throw std::invalid_argument(msg);
    }

    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    // TODO: Implement use of fileNum to make sure each file is evenly split at around 1000 lines of items per text file for efficient lookup
    run(args, args.at(1), getFileNum());
}