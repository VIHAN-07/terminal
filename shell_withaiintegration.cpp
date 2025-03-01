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

// ✅ Winsock2.h must be included BEFORE windows.h
#include <winsock2.h>
#include <windows.h>

#include <chrono>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>  // Ensure you have this installed


namespace fs = std::filesystem;
using json = nlohmann::json;

// Global Storage
std::unordered_map<std::string, std::string> variables;
std::unordered_map<std::string, std::function<std::string(std::vector<std::string>)>> functions;
std::string groq_api_key;

// CURL callback for receiving data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch(std::bad_alloc &e) {
        return 0;
    }
}

// Logging System with timestamp formatting
void log_message(const std::string &msg) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &time);
    
    std::ofstream logFile("myshell.log", std::ios::app);
    logFile << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "] " << msg << std::endl;
}

// Improved Tokenizer that handles quoted strings
std::vector<std::string> tokenize(const std::string &input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    bool inQuotes = false;
    std::string quotedString;
    
    while (stream >> std::quoted(token)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Error Handling
void show_error(const std::string &msg) {
    std::cerr << "\033[1;31m[Error]\033[0m " << msg << std::endl;
    log_message("ERROR: " + msg);
}

// Execute External Commands with output capture
std::string execute_command(const std::string &cmd) {
    std::string result;
    char buffer[128];
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) {
        show_error("Command failed to start: " + cmd);
        return "Error executing command";
    }
    
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        show_error("Exception while reading command output: " + cmd);
        return "Error reading command output";
    }
    
    int status = _pclose(pipe);
    if (status != 0) {
        show_error("Command exited with status " + std::to_string(status) + ": " + cmd);
    }
    
    return result;
}

// File Handling with error checking
std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        show_error("File not found: " + filename);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string &filename, const std::string &content) {
    std::ofstream file(filename);
    if (!file) {
        show_error("Cannot write to file: " + filename);
        return;
    }
    file << content;
    file.close();
    std::cout << "Content written to " << filename << std::endl;
}

void append_file(const std::string &filename, const std::string &content) {
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        show_error("Cannot append to file: " + filename);
        return;
    }
    file << content << "\n";
    file.close();
    std::cout << "Content appended to " << filename << std::endl;
}

// Directory Commands
void change_directory(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: cd <directory>");
        return;
    }
    try {
        fs::current_path(tokens[1]);
        std::cout << "Changed directory to: " << fs::current_path().string() << std::endl;
    } catch (const fs::filesystem_error& e) {
        show_error("Directory error: " + std::string(e.what()));
    } catch (...) {
        show_error("Directory not found: " + tokens[1]);
    }
}

void list_directory(const std::string& path = ".") {
    try {
        std::string targetPath = path == "." ? fs::current_path().string() : path;
        std::cout << "Contents of " << targetPath << ":\n";
        
        // Column formatting variables
        int nameColWidth = 30;
        int sizeColWidth = 10;
        int typeColWidth = 10;
        
        // Print header
        std::cout << std::left << std::setw(nameColWidth) << "Name" 
                  << std::setw(sizeColWidth) << "Size" 
                  << std::setw(typeColWidth) << "Type" << std::endl;
        std::cout << std::string(nameColWidth + sizeColWidth + typeColWidth, '-') << std::endl;
        
        for (const auto &entry : fs::directory_iterator(targetPath)) {
            std::string name = entry.path().filename().string();
            std::string size = entry.is_directory() ? "-" : std::to_string(fs::file_size(entry.path())) + "B";
            std::string type = entry.is_directory() ? "Dir" : "File";
            
            std::cout << std::left << std::setw(nameColWidth) << name 
                      << std::setw(sizeColWidth) << size 
                      << std::setw(typeColWidth) << type << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        show_error("Filesystem error: " + std::string(e.what()));
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
    } catch (const fs::filesystem_error& e) {
        show_error("Filesystem error: " + std::string(e.what()));
    } catch (...) {
        show_error("Error creating directory: " + path);
    }
}

void remove_file_or_directory(const std::string &path) {
    try {
        if (fs::is_directory(path)) {
            std::cout << "Removing directory: " << path << std::endl;
            std::uintmax_t n = fs::remove_all(path);
            std::cout << "Removed " << n << " files/directories" << std::endl;
        } else {
            if (fs::remove(path)) {
                std::cout << "File removed: " << path << std::endl;
            } else {
                show_error("Failed to remove file: " + path);
            }
        }
    } catch (const fs::filesystem_error& e) {
        show_error("Filesystem error: " + std::string(e.what()));
    } catch (...) {
        show_error("Error deleting file: " + path);
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
        if (!line.empty() && line[0] != '#') { // Skip comments
            lines.push_back(line);
        }
    }
    file.close();
    std::cout << "Imported " << lines.size() << " commands from " << filename << std::endl;
    // Will be executed in execute_script function
    return;
}

// Variable Assignment and Expansion
std::string expand_variables(const std::string &input) {
    std::string result = input;
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
        // Find the end of the variable name
        size_t end = pos + 1;
        while (end < result.length() && (isalnum(result[end]) || result[end] == '_')) {
            end++;
        }
        
        if (end > pos + 1) {
            std::string varName = result.substr(pos + 1, end - pos - 1);
            if (variables.count(varName)) {
                result.replace(pos, end - pos, variables[varName]);
                // Don't increment pos here, as we need to check for variables in the replacement
            } else {
                // Variable not found, replace with empty string
                result.replace(pos, end - pos, "");
            }
        } else {
            // Lone $ character, just skip it
            pos++;
        }
    }
    return result;
}

// Math Functions
std::string math_function(const std::string &func, double x) {
    if (func == "sin") return std::to_string(sin(x));
    if (func == "cos") return std::to_string(cos(x));
    if (func == "tan") return std::to_string(tan(x));
    if (func == "log") return std::to_string(log(x));
    if (func == "exp") return std::to_string(exp(x));
    if (func == "sqrt") return std::to_string(sqrt(x));
    if (func == "round") return std::to_string(round(x));
    if (func == "floor") return std::to_string(floor(x));
    if (func == "ceil") return std::to_string(ceil(x));
    
    show_error("Unknown math function: " + func);
    return "0";
}

// Basic calculator function
double calculate(const std::string &expression) {
    // This is a very simplified calculator, would need a proper parser for a real one
    // But for basic expressions, this can work
    
    std::istringstream ss(expression);
    double left, right;
    char op;
    
    ss >> left;
    ss >> op;
    ss >> right;
    
    switch (op) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': 
            if (right == 0) {
                show_error("Division by zero");
                return 0;
            }
            return left / right;
        default:
            show_error("Unknown operator: " + std::string(1, op));
            return 0;
    }
}

// Initialize Groq API
bool init_groq_api() {
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Check if API key is set
    if (variables.find("GROQ_API_KEY") != variables.end()) {
        groq_api_key = variables["GROQ_API_KEY"];
        std::cout << "\033[1;32mGroq AI API enabled\033[0m" << std::endl;
        return true;
    }
    
    std::cout << "\033[1;33mGroq AI API not configured. Use 'set GROQ_API_KEY your_api_key' to enable AI features.\033[0m" << std::endl;
    return false;
}

// Make API request to Groq
std::string call_groq_api(const std::string &prompt, const std::string &model = "llama3-70b-8192") {
    if (groq_api_key.empty()) {
        show_error("Groq API key not set. Use 'set GROQ_API_KEY your_api_key' to enable AI features.");
        return "Error: API key not configured";
    }
    
    CURL *curl = curl_easy_init();
    std::string readBuffer;
    
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + groq_api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
        
        // Create the request JSON
        json request_data = {
            {"model", model},
            {"messages", json::array({
                {{"role", "user"}, {"content", prompt}}
            })},
            {"temperature", 0.7},
            {"max_tokens", 1024}
        };
        
        std::string request_body = request_data.dump();
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        std::cout << "Asking AI... " << std::flush;
        
        CURLcode res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            show_error("Groq API request failed: " + std::string(curl_easy_strerror(res)));
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return "Error calling Groq API";
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        try {
            // Parse the JSON response
            json response = json::parse(readBuffer);
            
            if (response.contains("choices") && response["choices"].size() > 0 &&
                response["choices"][0].contains("message") && 
                response["choices"][0]["message"].contains("content")) {
                
                std::string result = response["choices"][0]["message"]["content"];
                return result;
            } else if (response.contains("error") && response["error"].contains("message")) {
                return "API Error: " + response["error"]["message"].get<std::string>();
            } else {
                return "Error parsing response from Groq API";
            }
        } catch (const std::exception& e) {
            show_error("Failed to parse Groq API response: " + std::string(e.what()));
            return "Error parsing response";
        }
    }
    
    return "Error initializing CURL";
}

// AI Command Implementation
std::string ai_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: ai <prompt>");
        return "";
    }
    
    // Combine all tokens after "ai" into the prompt
    std::string prompt;
    for (size_t i = 1; i < tokens.size(); i++) {
        prompt += tokens[i] + " ";
    }
    
    std::string model = variables.count("AI_MODEL") ? variables["AI_MODEL"] : "llama3-70b-8192";
    return call_groq_api(prompt, model);
}

// AI Code Command
std::string ai_code_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        show_error("Usage: aicode <language> <description>");
        return "";
    }
    
    std::string language = tokens[1];
    
    // Combine all tokens after language into the description
    std::string description;
    for (size_t i = 2; i < tokens.size(); i++) {
        description += tokens[i] + " ";
    }
    
    std::string prompt = "Write a " + language + " program that " + description + 
                          ". Provide only the code without explanations.";
    
    std::string model = variables.count("AI_MODEL") ? variables["AI_MODEL"] : "llama3-70b-8192";
    return call_groq_api(prompt, model);
}

// AI Explain Code Command
std::string ai_explain_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: aiexplain <file.cpp>");
        return "";
    }
    
    std::string filename = tokens[1];
    std::string code = read_file(filename);
    
    if (code.empty()) {
        return "Could not read file: " + filename;
    }
    
    std::string prompt = "Explain the following code in detail:\n\n" + code;
    
    std::string model = variables.count("AI_MODEL") ? variables["AI_MODEL"] : "llama3-70b-8192";
    return call_groq_api(prompt, model);
}

// AI Fix Code Command
std::string ai_fix_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        show_error("Usage: aifix <file.cpp>");
        return "";
    }
    
    std::string filename = tokens[1];
    std::string code = read_file(filename);
    
    if (code.empty()) {
        return "Could not read file: " + filename;
    }
    
    std::string prompt = "Fix errors and improve the following code:\n\n" + code + 
                         "\n\nPlease provide only the corrected code without explanations.";
    
    std::string model = variables.count("AI_MODEL") ? variables["AI_MODEL"] : "llama3-70b-8192";
    std::string result = call_groq_api(prompt, model);
    
    // Ask user if they want to overwrite the file
    std::cout << "Do you want to overwrite " << filename << " with the fixed code? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);
    
    if (answer == "y" || answer == "Y") {
        write_file(filename, result);
        std::cout << "File " << filename << " updated with fixed code." << std::endl;
    }
    
    return result;
}

// AI Generate Command completion
std::string ai_complete_command(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        show_error("Usage: aicomplete <language> \"<partial code>\"");
        return "";
    }
    
    std::string language = tokens[1];
    std::string partial_code;
    
    // Combine all tokens after language into the partial code
    for (size_t i = 2; i < tokens.size(); i++) {
        partial_code += tokens[i] + " ";
    }
    
    std::string prompt = "Complete the following " + language + " code:\n\n" + partial_code + 
                         "\n\nProvide only the completed code.";
    
    std::string model = variables.count("AI_MODEL") ? variables["AI_MODEL"] : "llama3-70b-8192";
    return call_groq_api(prompt, model);
}

// Process a single command
void process_command(const std::string &command) {
    if (command.empty()) return;
    
    // Skip comments
    if (command[0] == '#') return;
    
    std::string expandedCommand = expand_variables(command);
    std::vector<std::string> tokens = tokenize(expandedCommand);
    
    if (tokens.empty()) return;
    
    // Command processing
    if (tokens[0] == "echo") {
        for (size_t i = 1; i < tokens.size(); i++) {
            std::cout << tokens[i] << " ";
        }
        std::cout << std::endl;
    }
    else if (tokens[0] == "set" || tokens[0] == "let") {
        if (tokens.size() < 3) {
            show_error("Usage: set <variable> <value>");
            return;
        }
        std::string value;
        for (size_t i = 2; i < tokens.size(); i++) {
            value += tokens[i] + " ";
        }
        if (!value.empty()) value.pop_back(); // Remove trailing space
        variables[tokens[1]] = value;
        
        // If setting GROQ_API_KEY, initialize API
        if (tokens[1] == "GROQ_API_KEY") {
            groq_api_key = value;
            std::cout << "\033[1;32mGroq AI API key set. AI features are now enabled.\033[0m" << std::endl;
        }
        
        std::cout << "Variable " << tokens[1] << " set to: " << value << std::endl;
    }
    else if (tokens[0] == "calc") {
        if (tokens.size() < 2) {
            show_error("Usage: calc <expression>");
            return;
        }
        std::string expr;
        for (size_t i = 1; i < tokens.size(); i++) {
            expr += tokens[i] + " ";
        }
        if (!expr.empty()) expr.pop_back(); // Remove trailing space
        double result = calculate(expr);
        std::cout << expr << " = " << result << std::endl;
    }
    else if (tokens[0] == "read") {
        if (tokens.size() < 3) {
            show_error("Usage: read <variable> <file>");
            return;
        }
        std::string content = read_file(tokens[2]);
        variables[tokens[1]] = content;
        std::cout << "Read file content into variable " << tokens[1] << std::endl;
    }
    else if (tokens[0] == "write") {
        if (tokens.size() < 3) {
            show_error("Usage: write <file> <content>");
            return;
        }
        std::string content;
        for (size_t i = 2; i < tokens.size(); i++) {
            content += tokens[i] + " ";
        }
        if (!content.empty()) content.pop_back(); // Remove trailing space
        write_file(tokens[1], content);
    }
    else if (tokens[0] == "append") {
        if (tokens.size() < 3) {
            show_error("Usage: append <file> <content>");
            return;
        }
        std::string content;
        for (size_t i = 2; i < tokens.size(); i++) {
            content += tokens[i] + " ";
        }
        if (!content.empty()) content.pop_back(); // Remove trailing space
        append_file(tokens[1], content);
    }
    else if (tokens[0] == "cd") {
        change_directory(tokens);
    }
    else if (tokens[0] == "ls" || tokens[0] == "dir") {
        if (tokens.size() > 1) {
            list_directory(tokens[1]);
        } else {
            list_directory();
        }
    }
    else if (tokens[0] == "mkdir") {
        if (tokens.size() < 2) {
            show_error("Usage: mkdir <directory>");
            return;
        }
        create_directory(tokens[1]);
    }
    else if (tokens[0] == "rm" || tokens[0] == "del") {
        if (tokens.size() < 2) {
            show_error("Usage: rm <file_or_directory>");
            return;
        }
        remove_file_or_directory(tokens[1]);
    }
    else if (tokens[0] == "import") {
        if (tokens.size() < 2) {
            show_error("Usage: import <scriptfile>");
            return;
        }
        import_script(tokens[1]);
    }
    else if (tokens[0] == "sleep") {
        if (tokens.size() < 2) {
            show_error("Usage: sleep <milliseconds>");
            return;
        }
        try {
            int ms = std::stoi(tokens[1]);
            std::cout << "Sleeping for " << ms << "ms..." << std::endl;
            Sleep(ms);
        } catch (...) {
            show_error("Invalid sleep time: " + tokens[1]);
        }
    }
    // AI Commands
    else if (tokens[0] == "ai") {
        std::string response = ai_command(tokens);
        std::cout << "\n\033[1;36m" << response << "\033[0m\n" << std::endl;
    }
    else if (tokens[0] == "aicode") {
        std::string code = ai_code_command(tokens);
        std::cout << "\n\033[1;32m" << code << "\033[0m\n" << std::endl;
        
        // Ask user if they want to save the code to a file
        std::cout << "Do you want to save this code to a file? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);
        
        if (answer == "y" || answer == "Y") {
            std::cout << "Enter filename: ";
            std::string filename;
            std::getline(std::cin, filename);
            write_file(filename, code);
        }
    }
    else if (tokens[0] == "aiexplain") {
        std::string explanation = ai_explain_command(tokens);
        std::cout << "\n\033[1;36m" << explanation << "\033[0m\n" << std::endl;
    }
    else if (tokens[0] == "aifix") {
        std::string fixed_code = ai_fix_command(tokens);
        std::cout << "\n\033[1;32m" << fixed_code << "\033[0m\n" << std::endl;
    }
    else if (tokens[0] == "aicomplete") {
        std::string completed_code = ai_complete_command(tokens);
        std::cout << "\n\033[1;32m" << completed_code << "\033[0m\n" << std::endl;
    }
    else if (tokens[0] == "aimodels") {
        std::cout << "\nAvailable Groq AI Models:\n";
        std::cout << "---------------------\n";
        std::cout << "llama3-70b-8192     - Llama 3 70B (default)\n";
        std::cout << "llama3-8b-8192      - Llama 3 8B (faster)\n";
        std::cout << "mixtral-8x7b-32768  - Mixtral 8x7B\n";
        std::cout << "gemma-7b-it         - Google Gemma 7B\n\n";
        std::cout << "To change model: set AI_MODEL model_name\n";
    }
    else if (tokens[0] == "help") {
        std::cout << "\nMyShell Commands:\n";
        std::cout << "----------------\n";
        std::cout << "echo <text>              - Print text to console\n";
        std::cout << "set/let <var> <value>    - Set variable value\n";
        std::cout << "calc <expression>        - Calculate simple expression\n";
        std::cout << "cd <directory>           - Change directory\n";
        std::cout << "ls/dir [directory]       - List directory contents\n";
        std::cout << "mkdir <directory>        - Create directory\n";
        std::cout << "rm/del <path>            - Remove file or directory\n";
        std::cout << "read <var> <file>        - Read file into variable\n";
        std::cout << "write <file> <content>   - Write content to file\n";
        std::cout << "append <file> <content>  - Append content to file\n";
        std::cout << "run <script>             - Run a script file\n";
        std::cout << "import <script>          - Import a script file\n";
        std::cout << "sleep <ms>               - Sleep for milliseconds\n";
        std::cout << "exit                     - Exit the shell\n";
        std::cout << "help                     - Show this help\n";
        
        if (!groq_api_key.empty()) {
            std::cout << "\nAI Commands:\n";
            std::cout << "------------\n";
            std::cout << "ai <prompt>                   - Ask AI a question\n";
            std::cout << "aicode <lang> <description>   - Generate code in specified language\n";
            std::cout << "aiexplain <file>              - Explain code in a file\n";
            std::cout << "aifix <file>                  - Fix and improve code in a file\n";
            std::cout << "aicomplete <lang> <code>      - Complete partial code\n";
            std::cout << "aimodels                      - List available AI models\n";
        } else {
            std::cout << "\nTo enable AI features, use: set GROQ_API_KEY your_api_key\n";
        }
    }
    else {
        // If not a built-in command, try to execute it as an external command
        std::string output = execute_command(expandedCommand);
        std::cout << output;
    }
}

// Execute Script
void execute_script(const std::vector<std::string> &lines) {
    for (const std::string &line : lines) {
        process_command(line);
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
    
    std::cout << "Running script " << filename << " (" << lines.size() << " commands)" << std::endl;
    execute_script(lines);
    std::cout << "Script execution completed" << std::endl;
}

// Run Shell
void run_shell() {
    // Set some environment variables
    variables["PATH"] = getenv("PATH") ? getenv("PATH") : "";
    variables["USER"] = getenv("USERNAME") ? getenv("USERNAME") : "user";
    variables["HOME"] = getenv("USERPROFILE") ? getenv("USERPROFILE") : ".";
    variables["SHELL"] = "MyShell";
    variables["AI_MODEL"] = "llama3-70b-8192";

    // Display welcome message
    std::cout << "\n==========================================================\n";
    std::cout << "                 MyShell v1.0                             \n";
    std::cout << "==========================================================\n";
    std::cout << "Type 'help' for available commands\n\n";
    
    // Initialize Groq API if possible
    init_groq_api();
    
    // Main command loop
    std::string input;
    while (true) {
        // Display prompt
        std::string currentDir = fs::current_path().string();
        std::cout << "\033[1;33m" << variables["USER"] << "@MyShell\033[0m:\033[1;34m" << currentDir << "\033[0m$ ";
        
        // Get input
        if (!std::getline(std::cin, input)) {
            break; // Exit on EOF
        }
        
        // Log the command
        log_message("Command executed: " + input);
        
        // Exit condition
        if (input == "exit" || input == "quit") {
            std::cout << "Exiting MyShell. Goodbye!" << std::endl;
            break;
        }
        
        // Process the command
        try {
            process_command(input);
        } catch (const std::exception& e) {
            show_error("Exception while processing command: " + std::string(e.what()));
        } catch (...) {
            show_error("Unknown exception while processing command");
        }
    }
    
    // Cleanup
    curl_global_cleanup();
}

int main(int argc, char* argv[]) {
    // Set console to support colors
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create log file if it doesn't exist
    std::ofstream logFile("myshell.log", std::ios::app);
    std::time_t now = std::time(nullptr);  // Get current time
logFile << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] MyShell started\n";

    logFile.close();
    
    // Register built-in functions
    functions["time"] = [](const std::vector<std::string>& args) -> std::string {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_s(&tm_buf, &time);
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%H:%M:%S");
        return ss.str();
    };
    
    functions["date"] = [](const std::vector<std::string>& args) -> std::string {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_s(&tm_buf, &time);
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d");
        return ss.str();
    };
    
    functions["random"] = [](const std::vector<std::string>& args) -> std::string {
        int min = 0, max = 100;
        if (args.size() >= 2) {
            try {
                min = std::stoi(args[0]);
                max = std::stoi(args[1]);
            } catch (...) {
                return "Error: Invalid arguments for random";
            }
        }
        int result = min + (std::rand() % (max - min + 1));
        return std::to_string(result);
    };
    
    // Check if a script file was provided as an argument
    if (argc > 1) {
        std::string scriptFile = argv[1];
        std::cout << "Running script file: " << scriptFile << std::endl;
        run_script(scriptFile);
        return 0;
    }
    
    // Run interactive shell
    run_shell();
    
    return 0;
}