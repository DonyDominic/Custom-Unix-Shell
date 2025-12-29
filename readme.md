# 🐚 Custom Bash Shell

A robust, modular Unix shell implementation in C. This project simulates a professional terminal environment, handling command-line parsing, process lifecycle management, and complex operator logic.



## 🎯 Project Overview
This shell was developed to explore low-level systems programming. It bridges the gap between user input and the Linux Kernel by utilizing system calls to manage processes and file descriptors.

### Key Features
- **Standard Command Execution:** Runs all system binaries (e.g., `ls`, `grep`, `vim`, `cat`).
- **Piping (`|`):** Implements Inter-Process Communication (IPC) by connecting the `stdout` of one process to the `stdin` of another.
- **I/O Redirection:** Supports input redirection (`<`), output redirection (`>`), and appending (`>>`).
- **Logical Operators:** Handles conditional execution sequences using `&&` (AND) and `||` (OR).

---

## 🏗️ Architecture & File Structure
The project is built with a modular "Separation of Concerns" approach:

```text
.
├── include/        # Header files (.h) defining data structures and prototypes
├── src/            # Core logic implementation (.c)
│   ├── tokens.c    # Lexical analysis (breaking input into words)
│   ├── parser.c    # Syntax analysis (identifying pipes and redirects)
│   ├── exec.c      # Process management (fork, exec, wait)
│   └── alias.c     # Alias mapping and lookup logic
├── utils/          # Shared helper functions for string and memory safety
└── main/           # Entry point and REPL loop