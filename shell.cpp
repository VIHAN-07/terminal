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

// Improved tokenizer to handle quotes and escape sequences
std::vector<std::string> tokenize(const std::string &input) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
    bool escaped = false;
    
    for (size_t i = 0; i < input.length(); i++) {
        if (escaped) {
            token += input[i];
            escaped = false;
            continue;
        }
        
        if (input[i] == '\\') {
            escaped = true;
            token += input[i];
            continue;
        }
        
        if (input[i] == '"') {
            in_quotes = !in_quotes;
            continue;
        }
        
        if (input[i] == ' ' && !in_quotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += input[i];
        }
    }
    
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Error Handling
void show_error(const std::string &msg) {
    std::cerr << "[Error] " << msg << std::endl;
    log_message("ERROR: " + msg);
}

// Execute External Commands with proper output handling
std::string execute_command(const std::string &cmd) {
    std::string result;
    char buffer[128];
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) {
        return "Error executing command.";
    }
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    _pclose(pipe);
    return result;
}

// File Handling
std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) return "Error: File not found.";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string process_escape_sequences(const std::string& input) {
    std::string output;
    bool escaped = false;
    
    for (size_t i = 0; i < input.length(); i++) {
        if (escaped) {
            switch (input[i]) {
                case 'n': output += '\n'; break;
                case 't': output += '\t'; break;
                case 'r': output += '\r'; break;
                default: output += input[i]; break;
            }
            escaped = false;
        } else if (input[i] == '\\') {
            escaped = true;
        } else {
            output += input[i];
        }
    }
    
    return output;
}

void write_file(const std::string &filename, const std::vector<std::string>& tokens) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        show_error("Failed to open file for writing: " + filename);
        return;
    }

    // Combine all tokens after the filename, preserving spaces
    std::string content;
    for (size_t i = 2; i < tokens.size(); i++) {
        content += tokens[i];
        // Add space between tokens, but not at the end
        if (i < tokens.size() - 1) content += " ";
    }

    // Process escape sequences
    content = process_escape_sequences(content);

    file << content;
    file.close();
    std::cout << "File written successfully: " << filename << std::endl;
}

void append_file(const std::string &filename, const std::string &content) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << content << "\n";
        file.close();
    } else {
        show_error("Failed to append to file: " + filename);
    }
}

// Directory Commands
void change_directory(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: cd <directory>");
        return;
    }
    try {
        fs::current_path(tokens[1]);
        std::cout << "Changed directory to: " << fs::current_path() << std::endl;
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

// Execute Script
void execute_script(const std::vector<std::string> &lines) {
    for (const std::string &line : lines) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments
        
        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) continue;

        std::cout << "Executing: " << line << std::endl;

        if (tokens[0] == "echo") {
            std::string message;
            for (size_t i = 1; i < tokens.size(); i++) {
                if (tokens[i][0] == '$') {
                    std::string varName = tokens[i].substr(1);
                    message += variables.count(varName) ? variables[varName] : "[Undefined: $" + varName + "]";
                } else {
                    message += tokens[i];
                }
                if (i < tokens.size() - 1) message += " ";
            }
            std::cout << process_escape_sequences(message) << std::endl;
        }
        else if (tokens[0] == "mkdir") {
            if (tokens.size() < 2) show_error("Usage: mkdir <directory>");
            else create_directory(tokens[1]);
        }
        else if (tokens[0] == "cd") {
            if (tokens.size() < 2) show_error("Usage: cd <directory>");
            else change_directory(tokens);
        }
        else if (tokens[0] == "write") {
            if (tokens.size() < 3) show_error("Usage: write <file> <content>");
            else write_file(tokens[1], tokens);
        }
        else if (tokens[0] == "ls" || tokens[0] == "dir") {
            list_directory();
        }
        else if (tokens[0] == "rm" || tokens[0] == "del") {
            if (tokens.size() < 2) show_error("Usage: rm <filename>");
            else remove_file(tokens[1]);
        }
        else {
            // For external commands, execute and show output
            std::string output = execute_command(line);
            std::cout << output;
            if (!output.empty() && output[output.length()-1] != '\n') {
                std::cout << std::endl;
            }
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
    std::cout << "Current directory: " << fs::current_path() << std::endl;
    
    while (true) {
        std::cout << "MyShell> ";
        std::getline(std::cin, command);
        
        if (command == "exit") break;
        if (command.empty()) continue;

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