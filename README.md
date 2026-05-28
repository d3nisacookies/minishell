# minishell
Making minimalist shell from the 42 cursus.


# Minishell

## Introduction
- What Minishell is
   Minishell is a group project that aims to recreate a basic Unix shell program in C. 
The project involves implementing several features and behaviours found in Bash, 
including pipes, redirections, signal handling, environment variables and variable 
expansion.

Minishell must also support built-in commands such as `cd`, `echo`, `env`, `exit`, 
`export`, `pwd` and `unset`.

## Main Supported Features

Minishell currently supports:

- An interactive command prompt using `readline`, including command history.
- Execution of external commands such as `ls`, `grep` and `cat`.
- Built-in commands:
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `exit`
- Environment variable management, including updating `PWD` and `OLDPWD`
  when changing directories.
- Variable expansion in `echo`, including environment variables such as
  `$HOME` and the most recent exit status through `$?`.
- Pipes (`|`) for connecting the output of one command to the input of another.
- Redirections:
  - Input redirection: `<`
  - Output redirection: `>`
  - Append redirection: `>>`
- Basic quote handling for single and double quotes.
- Signal behaviour for `Ctrl-C` and `Ctrl-\`.
- End-of-file handling through `Ctrl-D`.
- Exit status tracking for executed commands.

## Features
- Prompt and command history
- Builtins
- Environment variables
- Pipes
- Redirections
- Quote handling
- Signal handling
- Exit status

## How the Shell Works: Command Walkthrough
1. Reading user input
    We take user input by using readline after this we will tockize it (lexer).
2. Splitting command sequences
3. Parsing pipes
4. Parsing words, quotes and redirections
5. Deciding between builtin and external command
6. Executing commands
7. Handling pipes and redirections
8. Updating `$?`

## Technical Deep Dives
- Parsing
- Builtins
- Environment Variables
- Redirections
- Pipes
- Signals
- Exit Status and Error Handling
- Memory Management

## Example Commands
## Compilation and Usage
## Known Limitations / Remaining Work
## Authors





### Why We Have Builtins and External Commands

Commands in Minishell can either be **builtins** or **external commands**.

Builtins are commands implemented directly inside the shell. Some builtins, such as `cd`, `export`, `unset`, and `exit`, must be executed in the parent process because they change the state of the shell itself. For example, `cd` must change Minishell's current working directory. If it were executed in a child process, only the child's directory would change, and Minishell would remain in the original directory.

External commands, such as `ls`, `cat`, and `grep`, are separate executable programs stored on the system. Minishell runs these commands in a child process using `fork()` and `execve()`. This is necessary because `execve()` replaces the current process with the requested program. If Minishell executed an external command directly in the parent process, the shell itself would be replaced and would no longer be able to accept further commands.

Some builtins, such as `echo`, do not change the shell's internal state, but they are still implemented directly inside the shell.

One important case is redirection with builtins. For example:

```bash
echo hello > file1.txt
```

Because `echo` is executed inside Minishell, the shell temporarily redirects its standard output to `file1.txt`. After `echo` finishes, Minishell must restore its original standard output. Otherwise, any later output from the shell would continue to be written into `file1.txt` instead of appearing in the terminal.
