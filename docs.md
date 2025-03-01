# MyShell Documentation

## Overview

MyShell is a custom command-line shell implemented in C++ that provides standard shell functionality with additional features for file management, variable manipulation, and AI-assisted operations through the Groq API. The shell supports both interactive usage and script execution.

## Architecture

MyShell is structured around a command processing system that:
1. Parses user input into tokens
2. Executes built-in commands or forwards to external system commands
3. Manages variables, file operations, and directory navigation
4. Integrates with the Groq API for AI-powered features

## Core Components

### Command Processing

The core of MyShell is the command processing pipeline:
1. `run_shell()` - Main loop that displays prompts and accepts user input
2. `process_command()` - Processes a single command by tokenizing and dispatching
3. `tokenize()` - Splits input into tokens while respecting quoted strings
4. `expand_variables()` - Replaces variable references with their values

### File System Operations

MyShell implements various file system operations:
- `change_directory()` - Changes the current working directory
- `list_directory()` - Lists contents of a directory with formatting
- `create_directory()` - Creates a new directory
- `remove_file_or_directory()` - Removes files or directories recursively
- `read_file()` - Reads content of a file into memory
- `write_file()` - Writes content to a file
- `append_file()` - Appends content to an existing file

### Variable Management

Variables are stored in a global `std::unordered_map` and can be:
- Set using the `set`/`let` command
- Referenced using `$variable_name` syntax
- Read from files with the `read` command

### Script Execution

MyShell supports script execution in two ways:
- `run_script()` - Executes a script file from command line arguments
- `import_script()` - Imports and executes commands from another script file

### External Command Execution

Commands not recognized as built-in are passed to the system shell:
- `execute_command()` - Executes external commands and captures output

### Error Handling and Logging

MyShell implements robust error handling:
- `show_error()` - Displays formatted error messages
- `log_message()` - Logs commands and errors to a log file

### AI Integration

MyShell integrates with the Groq API for AI features:
- `init_groq_api()` - Initializes the Groq API connection
- `call_groq_api()` - Makes requests to the Groq API
- AI command implementations:
  - `ai_command()` - General AI assistant
  - `ai_code_command()` - Code generation
  - `ai_explain_command()` - Code explanation
  - `ai_fix_command()` - Code fixing
  - `ai_complete_command()` - Code completion

## Command Reference

### Basic Commands

| Command | Format | Description |
|---------|--------|-------------|
| `echo` | `echo <text>` | Print text to console |
| `set`/`let` | `set <var> <value>` | Set variable value |
| `calc` | `calc <expression>` | Calculate simple expression |
| `help` | `help` | Show help information |
| `exit`/`quit` | `exit` | Exit the shell |

### File Operations

| Command | Format | Description |
|---------|--------|-------------|
| `read` | `read <var> <file>` | Read file into variable |
| `write` | `write <file> <content>` | Write content to file |
| `append` | `append <file> <content>` | Append content to file |

### Directory Operations

| Command | Format | Description |
|---------|--------|-------------|
| `cd` | `cd <directory>` | Change directory |
| `ls`/`dir` | `ls [directory]` | List directory contents |
| `mkdir` | `mkdir <directory>` | Create directory |
| `rm`/`del` | `rm <path>` | Remove file or directory |

### Script Operations

| Command | Format | Description |
|---------|--------|-------------|
| `run` | `run <script>` | Run a script file |
| `import` | `import <script>` | Import a script file |
| `sleep` | `sleep <ms>` | Sleep for milliseconds |

### AI Commands

| Command | Format | Description |
|---------|--------|-------------|
| `ai` | `ai <prompt>` | Ask AI a question |
| `aicode` | `aicode <lang> <description>` | Generate code in specified language |
| `aiexplain` | `aiexplain <file>` | Explain code in a file |
| `aifix` | `aifix <file>` | Fix and improve code in a file |
| `aicomplete` | `aicomplete <lang> <code>` | Complete partial code |
| `aimodels` | `aimodels` | List available AI models |

## AI Features

### Configuration

To enable AI features, set the Groq API key:
```
set GROQ_API_KEY your_api_key
```

### Available Models

MyShell supports several AI models through Groq:
- `llama3-70b-8192` - Llama 3 70B (default)
- `llama3-8b-8192` - Llama 3 8B (faster)
- `mixtral-8x7b-32768` - Mixtral 8x7B
- `gemma-7b-it` - Google Gemma 7B

Change the model with:
```
set AI_MODEL model_name
```

### AI Commands Usage Examples

#### General AI Assistant
```
ai What is the capital of France?
```

#### Code Generation
```
aicode python write a function that calculates factorial
```

#### Code Explanation
```
aiexplain main.cpp
```

#### Code Fixing
```
aifix buggy.cpp
```

#### Code Completion
```
aicomplete javascript function calculateTax(amount) {
```

## Implementation Details

### Dependencies

MyShell relies on several external libraries:
- Windows-specific headers (`winsock2.h`, `windows.h`)
- `curl` for HTTP requests to the Groq API
- `nlohmann/json` for JSON parsing
- C++ standard library components

### Data Structures

Key data structures used:
- `std::unordered_map<std::string, std::string>` for variable storage
- `std::unordered_map<std::string, std::function<...>>` for function registry
- `std::vector<std::string>` for command token storage

### Error Handling

The code employs several error handling mechanisms:
- Try-catch blocks around external calls
- Error reporting via `show_error()`
- Logging of errors to a log file
- Function return values indicating success/failure

### Initialization and Setup

On startup, MyShell:
1. Sets up console for color output
2. Initializes the random seed
3. Creates log file if it doesn't exist
4. Registers built-in functions
5. Checks for script file arguments
6. Sets default environment variables
7. Initializes Groq API if possible

## Extension Points

MyShell can be extended in several ways:
1. Adding new built-in commands to `process_command()`
2. Registering new functions in the `functions` map
3. Adding new AI command implementations
4. Supporting additional file operations
5. Adding new mathematical functions to `math_function()`

## Limitations

- The calculator functionality is very basic and only supports simple expressions
- Error handling could be improved in some areas
- Limited support for command-line arguments and flags
- No advanced shell features like pipes, redirection, or job control
- No command history or auto-completion