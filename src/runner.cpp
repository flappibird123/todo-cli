#include "runner.hpp"

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <optional>
#include <cstddef>
#include <cstdlib>

#include "time.hpp"
#include "constants.hpp"

namespace fs = std::filesystem;

namespace {
    const char *getHomeDir() {
        const char *home = std::getenv("HOME");
        if (!home) {
            throw std::runtime_error("HOME environment variable not set\n");
        }
        return home;
    }

    std::ifstream openRFile(const std::string& name) {
        fs::path filePath = fs::path(getenv("HOME")) / ".local" / "state" / "todo" / name; 
        std::ifstream file(filePath);
        
        if (!file) {
            throw std::runtime_error("Failed to open file" + name + '\n');
        }
        return file;
    }

    std::ofstream openWFile(const std::string& filename) {
        fs::path filePath = fs::path(getenv("HOME")) / ".local" / "state" / "todo" / filename;
        std::ofstream file(filePath, std::ios::app);

        if (!file) {
            throw std::runtime_error("Failed to open file" + filename + '\n');
        }

        return file;
    }

    std::ofstream openTFile(const std::string& filename) {
        fs::path filePath = fs::path(getenv("HOME")) / ".local" / "state" / "todo" / filename;
        std::ofstream file(filePath);
        
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filename + '\n');
        }

        return file;
    }

    bool fileExists(const std::string& name) {
        const char *home = getHomeDir();
        fs::path filePath = fs::path(home) / ".local" / "state" / "todo" / name;
        return fs::exists(filePath) && fs::is_regular_file(filePath);
    }

    std::optional<std::string> grep(const std::string& filename, const std::string& grepText) {
        std::ifstream file = openRFile(filename);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(grepText) != std::string::npos) {
                return line;
            }
        }

        return std::nullopt;
    }

    void addItems(const std::vector<std::string>& args, std::ofstream& file) {
        for (size_t i = 2; i < args.size(); ++i) {
            file << args.at(i);
            file << "\n";
        }   
    }

    void replaceFileLines(const std::vector<int>& lineNums) {
        std::ifstream in = openRFile("1.txt");

        std::vector<std::string> lines;
        std::string line;

        while (std::getline(in, line)) {
            lines.push_back(line);
        }
        in.close();

        for (const auto &i : lineNums) {
            if (static_cast<size_t>(i) >= lines.size()) {
                throw std::runtime_error("Line number out of range");
            }
        }

        for (const auto &idx : lineNums) {
            if (static_cast<size_t>(idx) < lines.size()) {
                lines.at(idx).insert(0, consts::MARKED_STRING);
            }
        }

        std::ofstream out = openTFile("1.txt");
        for (const auto& l : lines) {
            out << l << "\n";
        }
    }
}

void run(const std::vector<std::string>& args, const std::string& mode, const std::string& fileNum) {
    // TODO: Implement use of fileNum to make sure each file is evenly split at around 1000 lines of items per text file for efficient lookup
    (void)fileNum;

    if (!fileExists("1.txt")) {
        std::ofstream file = openWFile("1.txt");
        file.close();
    }

    if (mode == "add") {
        if (auto t = grep("1.txt", Date::getDate())) {
            std::ofstream file = openWFile("1.txt");
            addItems(args, file);
        } else {
            std::ofstream file = openWFile("1.txt");
            file << Date::getDate();
            file << "\n";
            addItems(args, file);
        }
    } else if (mode == "list") {
        std::ifstream file = openRFile("1.txt");
        std::string date = Date::getDate();
        std::string res = Date::getDate();
        auto end = Date::parseDate(res);
        if (!end) {
            throw std::runtime_error("Failed to parse date\n");
        }
        end.value().addDay();
        std::string line;
        bool shouldPrint = false;
        while (std::getline(file, line)) {
            if (line == date) {
                shouldPrint = true;
                continue;
            } else if (line == end.value().toString()) {
                break;
            } else if (shouldPrint) {
                if (line.starts_with(consts::MARKED_STRING)) {
                    continue;
                }
                std::cout << line << "\n";
            }
        }
    } else if (mode == "mark") {
        if (args.size() < 3) {
            throw std::invalid_argument("Not enough arguments\n");
        }
        std::ifstream file = openRFile("1.txt");
        std::string date = Date::getDate();
        
        std::vector<int> lineNums;
        std::string line;
        for (size_t i = 2; i < args.size(); ++i) {
            int j = 0;
            while (std::getline(file, line)) {
                if (line == args.at(i)) {
                    lineNums.push_back(j);
                    break;
                }
                ++j;
            }
        }

        replaceFileLines(lineNums);
    } else {
        throw std::runtime_error("Invalid option: " + mode + '\n'); 
    }
    
}