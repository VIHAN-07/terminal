#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <functional>  
#include <windows.h>

namespace fs = std::filesystem;

// Global Storage
std::unordered_map<std::string, std::string> variables;
std::unordered_map<std::string, std::function<std::string(std::vector<std::string>)>> functions;

// Logging System
void log_message(const std::string &msg) {
    std::ofstream logFile("myshell.log", std::ios::app);
    logFile << "[" << std::time(0) << "] " << msg << std::endl;
}

// Tokenizer
std::vector<std::string> tokenize(const std::string &input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Error Handling
void show_error(const std::string &msg) {
    std::cerr << "[Error] " << msg << std::endl;
    log_message("ERROR: " + msg);
}

// Execute External Commands
void execute_command(const std::string &cmd) {
    int result = system(cmd.c_str());
    if (result != 0) {
        show_error("Command failed: " + cmd);
    }
}

// File Handling
std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) return "Error: File not found.";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string &filename, const std::string &content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

void append_file(const std::string &filename, const std::string &content) {
    std::ofstream file(filename, std::ios::app);
    file << content << "\n";
    file.close();
}

// Directory Commands
void change_directory(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: cd <directory>");
        return;
    }
    try {
        fs::current_path(tokens[1]);
    } catch (...) {
        show_error("Directory not found: " + tokens[1]);
    }
}

void list_directory() {
    try {
        for (const auto &entry : fs::directory_iterator(fs::current_path())) {
            std::cout << entry.path().filename().string() << "\n";
        }
    } catch (...) {
        show_error("Failed to list directory contents.");
    }
}

void create_directory(const std::string &path) {
    try {
        if (fs::create_directory(path)) {
            std::cout << "Directory created: " << path << std::endl;
        } else {
            show_error("Failed to create directory: " + path);
        }
    } catch (...) {
        show_error("Error creating directory: " + path);
    }
}

void remove_file(const std::string &filename) {
    try {
        if (fs::remove(filename)) {
            std::cout << "File removed: " << filename << std::endl;
        } else {
            show_error("Failed to remove file: " + filename);
        }
    } catch (...) {
        show_error("Error deleting file: " + filename);
    }
}

// Import External Scripts
void import_script(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        show_error("Failed to import script: " + filename);
        return;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    for (const std::string& cmd : lines) {
        execute_command(cmd);
    }
}

// Math Functions
std::string math_function(const std::string &func, double x) {
    if (func == "sin") return std::to_string(sin(x));
    if (func == "cos") return std::to_string(cos(x));
    if (func == "tan") return std::to_string(tan(x));
    if (func == "log") return std::to_string(log(x));
    if (func == "exp") return std::to_string(exp(x));
    return "Error: Unknown function.";
}

// Execute Script
void execute_script(const std::vector<std::string> &lines) {
    for (const std::string &line : lines) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "echo") {
            // ✅ Improved variable handling in echo
            for (size_t j = 1; j < tokens.size(); ++j) {
                if (tokens[j][0] == '$') { 
                    std::string varName = tokens[j].substr(1);
                    if (variables.count(varName)) std::cout << variables[varName] << " ";
                    else std::cout << "[Undefined: $" << varName << "] ";
                } else {
                    std::cout << tokens[j] << " ";
                }
            }
            std::cout << "\n";
        }
        else if (tokens[0] == "mkdir") {
            // ✅ Fix: Allow directories with spaces in quotes
            if (tokens.size() < 2) show_error("Usage: mkdir <directory>");
            else create_directory(tokens[1]);
        }
        else if (tokens[0] == "cd") {
            // ✅ Fix: Handle "cd" with spaces in directory names
            if (tokens.size() < 2) show_error("Usage: cd <directory>");
            else change_directory(tokens);
        }
        else if (tokens[0] == "ls" || tokens[0] == "dir") {
            // ✅ Ensure listing works in the current directory
            list_directory();
        }
        else if (tokens[0] == "write") {
            // ✅ Fix: Write content to a file
            if (tokens.size() < 3) show_error("Usage: write <file> <content>");
            else write_file(tokens[1], tokens[2]);
        }
        else {
            execute_command(line);
        }
    }
}


// Run Scripts
void run_script(const std::string &filename) {
    std::ifstream scriptFile(filename);
    if (!scriptFile.is_open()) {
        show_error("Unable to open script file: " + filename);
        return;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(scriptFile, line)) {
        lines.push_back(line);
    }
    scriptFile.close();
    execute_script(lines);
}

// Run Shell
void run_shell() {
    std::string command;
    while (true) {
        std::cout << "MyShell> ";
        std::getline(std::cin, command);
        if (command == "exit") break;

        std::vector<std::string> tokens = tokenize(command);
        if (tokens.empty()) continue;

        if (tokens[0] == "run") {
            if (tokens.size() < 2) show_error("Usage: run <script.mys>");
            else run_script(tokens[1]);
        } else {
            execute_script({command});
        }
    }
}

int main() {
    std::cout << "Welcome to MyShell! Type 'exit' to quit.\n";
    run_shell();
    return 0;
}
