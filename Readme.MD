### README: MyShell  

# MyShell  

MyShell is a lightweight command-line shell for Windows with support for basic shell commands, file handling, directory management, and script execution. It provides an interactive environment for executing system commands, custom scripting, and handling files easily.  

---

## Features  

- **Custom Command Execution**: Run commands like `echo`, `mkdir`, `cd`, `write`, `ls`, and `rm`.  
- **Script Execution**: Run `.mys` script files containing multiple shell commands.  
- **File Handling**: Read, write, and append to files easily.  
- **Error Logging**: Automatically logs errors to `myshell.log` for debugging.  
- **External Command Support**: Supports running system commands (`dir`, `ping`, etc.).  
- **Advanced Tokenization**: Handles quoted strings and escape sequences.  
- **Cross-Command Variable Support**: Store and use variables across commands.  

---

## Installation  

### Prerequisites  
- **Windows OS** (Tested on Windows 10/11)  
- **C++ Compiler** (MinGW/GCC or MSVC)  

### Compilation  
To compile MyShell, use the following command:  

```sh
g++ -std=c++17 -o myshell myshell.cpp
```

Ensure you have a compiler that supports C++17 or higher.

---

## Usage  

### Running MyShell  
Execute the compiled binary in a terminal:  

```sh
myshell.exe
```

### Running a Script  
To execute a `.mys` script file:  

```sh
run script.mys
```

---

## Supported Commands  

| Command | Description | Example |
|---------|-------------|---------|
| `echo` | Prints a message | `echo Hello World!` |
| `mkdir` | Creates a new directory | `mkdir myfolder` |
| `cd` | Changes the current directory | `cd myfolder` |
| `ls` or `dir` | Lists files in the current directory | `ls` |
| `write` | Writes content to a file | `write myfile.txt "Hello, MyShell!"` |
| `rm` or `del` | Removes a file | `rm myfile.txt` |
| `run` | Runs a `.mys` script file | `run script.mys` |
| `exit` | Exits MyShell | `exit` |

---

## Example Script (`script.mys`)  

```sh
# This is a MyShell script
echo "Welcome to MyShell!"
mkdir test_directory
cd test_directory
write sample.txt "This is a test file."
ls
cd ..
rm test_directory/sample.txt
```

---

## Error Logging  

MyShell logs errors to `myshell.log`. If you encounter an issue, check the log file for debugging.  

---

## Future Improvements  

- **More Built-in Commands**: Support for `cp`, `mv`, etc.  
- **Enhanced Scripting**: Variables, loops, and conditional support.  
- **Cross-Platform Compatibility**: Linux/macOS support.  

---

## License  

This project is open-source. You are free to modify and distribute it.  

---

Let me know if you want any refinements or additional sections! 🚀