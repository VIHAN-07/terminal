# MyShell

## Overview

MyShell is a powerful, customizable command-line shell written in C++ that combines traditional shell functionality with modern AI capabilities. It provides an intuitive interface for file management, script execution, and AI-assisted operations powered by the Groq API.

![MyShell Screenshot](https://via.placeholder.com/800x400?text=MyShell+Screenshot)

## Features

- **Core Shell Functions**: File navigation, directory management, and script execution
- **Variable System**: Define and use variables with simple `set` and `$variable` syntax
- **Built-in Calculator**: Perform mathematical operations directly in the shell
- **Script Support**: Run and import shell scripts for automation
- **Colorized Output**: Syntax highlighting and formatted display
- **Comprehensive Logging**: Automatic logging of all commands and errors
- **AI Integration**: Leverage Groq's large language models for:
  - Code generation in various programming languages
  - Code explanation and documentation
  - Code fixing and improvement
  - Intelligent command completion
  - General AI assistance

## Installation

### Prerequisites

- C++ compiler with C++17 support
- CMake 3.12 or higher
- libcurl
- nlohmann/json library
- Windows OS (currently Windows-specific)

### Building from Source

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/myshell.git
   cd myshell
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Run CMake and build:
   ```
   cmake ..
   cmake --build .
   ```

4. The executable will be available in the `build/bin` directory.

## Quick Start

1. Launch MyShell by running the executable:
   ```
   ./myshell
   ```

2. Use the built-in commands to navigate and manage files:
   ```
   ls
   mkdir test_dir
   cd test_dir
   ```

3. Set and use variables:
   ```
   set name "John Doe"
   echo Hello $name
   ```

4. To enable AI features, set your Groq API key:
   ```
   set GROQ_API_KEY your_api_key_here
   ```

5. Use AI commands for assistance:
   ```
   ai What is the Fibonacci sequence?
   aicode python "generate a function to calculate Fibonacci numbers"
   ```

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
| `cd` | `cd <directory>` | Change directory |
| `ls`/`dir` | `ls [directory]` | List directory contents |
| `mkdir` | `mkdir <directory>` | Create directory |
| `rm`/`del` | `rm <path>` | Remove file or directory |

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

MyShell integrates with Groq's API to provide AI capabilities. Available models include:

- `llama3-70b-8192` - Llama 3 70B (default)
- `llama3-8b-8192` - Llama 3 8B (faster)
- `mixtral-8x7b-32768` - Mixtral 8x7B
- `gemma-7b-it` - Google Gemma 7B

Change the active model with:
```
set AI_MODEL model_name
```

## Script Example

```bash
# example.msh - Simple MyShell script example
echo "Starting script execution..."
mkdir test_folder
cd test_folder
set current_dir $PWD
echo "Current directory: $current_dir"
write hello.txt "Hello, World!"
aicode python "print the first 10 Fibonacci numbers"
write fibonacci.py $RESULT
echo "Script completed successfully."
```

Run the script with:
```
run example.msh
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- [Groq](https://groq.com/) for their powerful LLM API
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing
- [libcurl](https://curl.se/libcurl/) for HTTP requests

## Contact

Project Link: [https://github.com/yourusername/myshell](https://github.com/yourusername/myshell)