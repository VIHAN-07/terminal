# MyShell User Guide

## Introduction

MyShell is a custom command-line interface that combines traditional shell functionality with advanced AI capabilities. It provides file management, variable operations, scripting support, and AI-powered code generation and assistance through integration with the Groq API.

## Getting Started

### Starting MyShell

Run the `myshell.exe` executable. You'll see a welcome message and command prompt:

```
==========================================================
                 MyShell v1.0                             
==========================================================
Type 'help' for available commands

user@MyShell:C:\current\directory$
```

### Running Scripts

To run a script file:
```
myshell.exe script.txt
```

## Basic Commands

| Command | Description | Example |
|---------|-------------|---------|
| `echo <text>` | Print text to console | `echo Hello World` |
| `set <var> <value>` or `let <var> <value>` | Set variable value | `set name John` |
| `calc <expression>` | Calculate simple expression | `calc 5 + 3` |
| `exit` or `quit` | Exit the shell | `exit` |
| `help` | Show help information | `help` |

## File and Directory Operations

| Command | Description | Example |
|---------|-------------|---------|
| `cd <directory>` | Change directory | `cd Documents` |
| `ls` or `dir` [directory] | List directory contents | `ls C:\Users` |
| `mkdir <directory>` | Create directory | `mkdir NewFolder` |
| `rm <path>` or `del <path>` | Remove file or directory | `rm oldfile.txt` |
| `read <var> <file>` | Read file into variable | `read content data.txt` |
| `write <file> <content>` | Write content to file | `write output.txt Hello World` |
| `append <file> <content>` | Append content to file | `append log.txt New entry` |

## Scripting and Control Flow

| Command | Description | Example |
|---------|-------------|---------|
| `run <script>` | Run a script file | `run myscript.txt` |
| `import <script>` | Import a script file | `import functions.txt` |
| `sleep <ms>` | Sleep for milliseconds | `sleep 1000` |

## Variables

Variables can be set with the `set` or `let` commands and referenced using the `$` symbol:

```
set greeting Hello
echo $greeting World
```

Built-in variables include:
- `$PATH` - System path
- `$USER` - Current username
- `$HOME` - User's home directory
- `$SHELL` - Current shell name (MyShell)
- `$AI_MODEL` - Selected AI model for Groq API

## AI Features

### Setup

To enable AI features, you must set your Groq API key:

```
set GROQ_API_KEY your_api_key_here
```

### AI Commands

| Command | Description | Example |
|---------|-------------|---------|
| `ai <prompt>` | Ask AI a question | `ai Explain quantum computing` |
| `aicode <lang> <description>` | Generate code in specified language | `aicode python "a simple web scraper"` |
| `aiexplain <file>` | Explain code in a file | `aiexplain script.cpp` |
| `aifix <file>` | Fix and improve code in a file | `aifix buggy.py` |
| `aicomplete <lang> <code>` | Complete partial code | `aicomplete javascript "function add(a, b) {"` |
| `aimodels` | List available AI models | `aimodels` |

### Changing AI Models

You can change the AI model used by setting the `AI_MODEL` variable:

```
set AI_MODEL llama3-8b-8192
```

Available models:
- `llama3-70b-8192` (default)
- `llama3-8b-8192` (faster)
- `mixtral-8x7b-32768`
- `gemma-7b-it`

## Interactive Features

### Tab Completion

MyShell supports tab completion for:

1. **Commands**: Press TAB after typing the first few letters of a command to complete it
   ```
   ec[TAB]  → echo
   ```

2. **Variables**: Press TAB after typing `$` to see available variables or complete a partially typed variable name
   ```
   echo $HO[TAB]  → echo $HOME
   ```

3. **Files and Directories**: Press TAB after typing part of a file or directory name to complete it
   ```
   cd Doc[TAB]  → cd Documents/
   ```

If multiple completions are possible, pressing TAB will:
- Complete up to the common characters
- Show all possible completions if pressed again

### Command History

Use the up and down arrow keys to navigate through previously entered commands:

- **Up Arrow**: Move backward through command history
- **Down Arrow**: Move forward through command history

## Advanced Features

### Command Tokenization
Commands with quoted strings are properly handled:
```
echo "This is a single argument with spaces"
```

### Error Handling
Errors are displayed in red with an [Error] prefix and logged to `myshell.log`.

### Logging
All commands are logged to `myshell.log` with timestamps.

## Examples

### Basic Operations
```
echo Hello World
set name Alice
echo Hello $name
calc 10 * 5
```

### File Operations
```
mkdir Projects
cd Projects
write hello.txt Hello, this is a test file
read content hello.txt
echo $content
append hello.txt This is a new line
```

### AI Examples
```
aicode python "a function that sorts a list of numbers"
ai Explain the difference between merge sort and quick sort
aiexplain main.cpp
aifix buggy.js
```

## Tips and Tricks

1. Use tab completion to quickly enter commands, variables, and file paths
2. Use the up/down arrow keys to recall and edit previous commands 
3. Variables can be used in any command with the `$` prefix
4. AI-generated code can be saved directly to a file when prompted
5. Set commonly used variables in a startup script and use `import` to load it

## Troubleshooting

1. If external commands fail, check that they're in your system PATH
2. If AI commands fail, verify your Groq API key is correct
3. Check `myshell.log` for detailed error information
4. Make sure you have an active internet connection for AI features

## Limitations

1. No piping or redirection operators
2. No aliases or custom functions
3. Limited error handling for complex scenarios
4. No control structures (if/else, loops)