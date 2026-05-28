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
    ### 1. Reading User Input

The first step is to read the command entered by the user. We use `readline()` to display the shell prompt and collect the full line of input as a string.

For example, if the user enters:

```bash
echo "hello world" > file.txt
```

`readline()` initially gives Minishell one complete string:

```c
"echo \"hello world\" > file.txt"
```

This string is then passed to the **lexer**, which tokenises the input. Tokenising means breaking the command into meaningful pieces, while recognising special operators and respecting quotes.

The example above would be separated into tokens such as:

```text
echo
"hello world"
>
file.txt
```

The lexer must understand that `"hello world"` is one argument, even though it contains a space, and that `>` is a redirection operator rather than a normal argument.

### 2. Splitting Command Sequences

After reading the user's input, Minishell passes the raw input string to `execute_input_segments()`. This function handles command sequences separated by semicolons.

For example:

```bash
echo hello ; pwd ; ls
```

is split into an array of separate command segments:

```text
segments[0] = "echo hello"
segments[1] = "pwd"
segments[2] = "ls"
segments[3] = NULL
```

If the input does not contain a semicolon, the function still returns an array, but it contains only one segment. For example:

```bash
echo hello
```

becomes:

```text
segments[0] = "echo hello"
segments[1] = NULL
```

Once the segments have been created, `validate_segments()` checks that the sequence is valid and does not contain syntax errors, such as an empty command between semicolons. Each valid segment is then passed individually to `parse_command()`, which fills the `t_cmd` structures needed for execution.

Finally, `execute_command()` runs each parsed command in order before moving on to the next segment.

3. Parsing pipes
### 3. Parsing Pipes

After command sequences have been separated, each individual segment is passed to `parse_command()`.

For example, after splitting the input:

```bash
echo start ; cat file.txt | grep hello > result.txt
```

the second segment is passed into the parser as:

```bash
cat file.txt | grep hello > result.txt
```

Inside `parse_command()`, the function `split_pipes()` checks whether the segment contains pipe operators (`|`). A pipe connects the output of one command to the input of the next command.

In this example:

```bash
cat file.txt | grep hello > result.txt
```

`split_pipes()` separates the segment into two command strings:

```text
pipeline[0] = "cat file.txt"
pipeline[1] = "grep hello > result.txt"
pipeline[2] = NULL
```

The parser must only split on valid pipe operators. A pipe character inside quotes is part of an argument and should not create a pipeline. For example:

```bash
echo "hello | world"
```

should remain a single command, because the `|` is inside quotes.

Once the pipeline has been split, each command string is individually parsed using `parse_single()`. This creates one `t_cmd` structure for each command in the pipeline.

For the example above, the result is conceptually:

```text
t_cmd 1:
    args = ["cat", "file.txt", NULL]
    next ──────────────────────┐
                               ↓
t_cmd 2:
    args = ["grep", "hello", NULL]
    outfile = "result.txt"
    next = NULL
```

The pipe operator itself does not need to remain as an argument inside the command structures. Instead, the pipeline is represented by linking the command structures together using the `next` pointer.

Therefore:

```bash
cat file.txt | grep hello > result.txt
```

becomes a linked list where the `cat` command points to the `grep` command:

```text
cat file.txt  →  grep hello > result.txt
```

At this point, the commands have only been parsed and connected structurally. The actual pipe file descriptors are created later during execution, when `execute_command()` detects that `cmd->next` exists and calls the pipeline execution logic.

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
