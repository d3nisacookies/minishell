*This project has been created as part of the 42 curriculum by jculleto, akaung*


# Description

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

## Different data strutures and what/why we chose our one


### AST — Abstract Syntax Tree

After tockinzation the parsing will use a precendence to decide how the groupings will connect, the pros of this is it can identify exactly what belongs to what wich creates a cleaner base for execution, however to create this system is quite difficult. Instead of spliting by pipes and by spaces and storing it all in a linked list. Ast parser wants to know things like: 

What is the main operator?
Which tokens belong on the left?
Which tokens belong on the right?
What is grouped together?
What has higher precedence?
Is the syntax valid?

### concrete syntax tree

A Concrete Syntax Tree is similar to an AST, but it represents the input much more literally. It keeps more of the original syntax, such as brackets, grammar rules, and extra intermediate nodes. This makes syntax validation easier because the tree closely follows the exact grammar of the language. However, because it keeps so much detail, it becomes more cluttered. That makes execution harder, because the executor has to move through many unnecessary layers before reaching the useful commands or operators.

### Linked list

We decided to use a simple linked list to represent our commands. Each node in the list contains one command, including its arguments, redirections, and other command information. The list is split by pipes, so each pipe creates another command node connected with `next`.

For example:

```bash
cat file.txt | grep hello | wc -l
```

would be represented as:

```text
[cat file.txt] -> [grep hello] -> [wc -l]
```

During execution, we move through the linked list from left to right. If a command has a `next` node, we know that its output needs to be connected to the next command through a pipe.

A pro of this approach is that it keeps the parser and executor relatively simple, because each command can be handled one at a time. A con is that it becomes harder to add more advanced shell features, such as bonus operators like `&&`, `||`, or parentheses, because the linked list does not naturally group commands by priority or relationship like an AST would.





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
   - Heredoc redirection: `<<`
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

Minishell starts by reading the user's command with `readline()`. This gives us the full input as one string.

For example:

```bash
echo "hello world" > file.txt
```

is first received as one full line. The parser then breaks it into meaningful parts, such as the command, arguments, quotes and redirection symbols.

The parser must understand that `"hello world"` is one argument, even though it contains a space, and that `>` is a redirection operator.

### 2. Splitting Command Sequences

After reading the input, Minishell checks for semicolons. Each semicolon separates one command sequence from the next.

For example:

```bash
echo hello ; pwd ; ls
```

becomes:

```text
echo hello
pwd
ls
```

Each segment is validated and then parsed separately. Minishell executes each valid segment in order.

### 3. Parsing Pipes

Each command segment is checked for pipes. A pipe connects the output of one command to the input of the next.

For example:

```bash
cat file.txt | grep hello > result.txt
```

is split into:

```text
cat file.txt
grep hello > result.txt
```

Each part becomes a `t_cmd` node. These nodes are connected using the `next` pointer, forming a linked list.

```text
[cat file.txt] -> [grep hello > result.txt]
```

The pipe itself is not stored as an argument. It is represented by the connection between command nodes.

### 4. Parsing Words, Quotes and Redirections

After pipes are split, each command is parsed into arguments and redirections.

For example:

```bash
grep "hello world" > result.txt
```

becomes:

```text
args = ["grep", "hello world", NULL]
output = result.txt
```

Quotes keep words together, so `"hello world"` is treated as one argument. Redirections like `<`, `>`, `>>`, and `<<` are stored in the command structure, not passed as normal arguments.

At this stage, Minishell only records the redirection. Files are opened later during execution.

### 5. Deciding Between Builtins and External Commands

Once a command has been parsed, Minishell decides how to execute it.

If the command has a `next` node, it is part of a pipeline and is sent to the pipeline executor.

If it is not a pipeline, Minishell checks whether it is a builtin. Builtins like `cd`, `export`, `unset` and `exit` must run inside the parent shell because they change the shell itself.

External commands like `ls`, `cat` and `grep` are run in child processes using `fork()` and `execvp()`.

### 6. Executing Commands

Execution starts after parsing is complete.

For a standalone builtin, Minishell runs the builtin directly. If it has redirections, Minishell saves the original input/output, applies the redirection, runs the builtin, then restores the original input/output.

If a command contains only redirections, Minishell still executes the redirection list. This matters for cases like:

```bash
<<EOF
hello
EOF
```

In that case the shell must enter heredoc mode and show the `> ` prompt even though there is no command name.

For an external command, Minishell creates a child process with `fork()`. The child applies redirections and then runs the program using `execve()`.

For a pipeline, Minishell creates pipes, forks one child per command, connects them with `dup2()`, then waits for them to finish.

### 7. Handling Pipes and Redirections

Pipes and redirections change where a command reads from and writes to.

```text
<   reads input from a file
>   writes output to a file, replacing old content
>>  writes output to a file, adding to the end
<<  reads lines until a delimiter and feeds them through stdin
|   sends one command's output into the next command
```

Minishell uses `open()` for files, `pipe()` for pipes, and `dup2()` to connect standard input and output to the correct place.

Unused file descriptors must be closed, otherwise commands may hang while waiting for input.

Heredoc input is collected before the command runs. `Ctrl-C` during heredoc stops collection and returns to the shell with status `130`.

### 8. Updating `$?`

Minishell stores the exit status of the last command in `shell->last_exit`.

A successful command usually sets it to `0`. A failed command sets it to a non-zero value.

For example:

```bash
echo $?
```

prints the value stored in `shell->last_exit`.

For pipelines, the final exit status usually comes from the last command in the pipeline. For syntax errors, Minishell should set the status to `2`. For signals, it usually stores `128 + signal number`, such as `130` for `Ctrl-C`.

When the shell is waiting for an external command or a pipeline, the parent temporarily ignores `SIGINT` and `SIGQUIT`. That prevents nested `./minishell` sessions from redrawing multiple `$> ` prompts when you press `Ctrl-C`.

### Why We Have Builtins and External Commands

Some commands must be built into the shell because they change the shell's own state.

For example, `cd` must change Minishell's current directory. If it ran only in a child process, the child would change directory and then exit, leaving Minishell in the same place.

External commands like `ls`, `cat` and `grep` are separate programs. Minishell runs them in child processes so the shell itself does not get replaced.

Builtins with redirections need extra care. Minishell must temporarily redirect its own input or output, run the builtin, and then restore the original terminal input/output.



#### Summary

`$?` is Minishell's memory of the result of the previous command.

```text
Successful command          → shell->last_exit = 0
Failed builtin              → shell->last_exit = 1 or another error code
Failed external command     → shell->last_exit = child's exit status
Command killed by signal    → shell->last_exit = 128 + signal number
Syntax error                → shell->last_exit = 2
Pipeline                    → shell->last_exit = status of the last command
```

By updating `shell->last_exit` after every command, Minishell can correctly support:

```bash
echo $?
```

and behave more like Bash.


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

# Instructions

## Requirements

Before compiling, make sure you have:

- `gcc`
- `make`
- the `readline` library installed

On Linux, if `readline` is missing, install the development package for your system.

```bash
sudo apt install libreadline-dev
```

## Compilation

Build the project with:

```bash
make
```

This creates the executable:

```bash
./minishell
```

To remove object files:

```bash
make clean
```

To remove object files and the executable:

```bash
make fclean
```

To fully rebuild the project:

```bash
make re
```

## Running Minishell

Start the shell with:

```bash
./minishell
```

You should see the minishell prompt:

```text
$>
```

From there, you can type commands like:

```bash
pwd
echo hello
ls -la
```

## Example Commands to Try

Basic commands:

```bash
pwd
echo hello world
env
```

Builtins:

```bash
cd ..
pwd
export NAME=minishell
echo $NAME
unset NAME
exit
```

Pipes:

```bash
ls -la | grep .c
cat README.md | grep minishell
```

Redirections:

```bash
echo hello > file.txt
cat < file.txt
echo again >> file.txt
cat file.txt
```

Quotes:

```bash
echo "hello world"
echo 'hello world'
echo "home is $HOME"
```

Exit status:

```bash
ls missing_file
echo $?
```

## Exiting Minishell

You can exit Minishell by running:

```bash
exit
```

You can also press `Ctrl-D` on an empty prompt.

## Memory Check

If you want to check for memory leaks, you can run:

```bash
valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
```

The suppression file is used because `readline` can report reachable memory that does not come from Minishell itself.


# Resources

https://dev.to/balapriya/abstract-syntax-tree-ast-explained-in-plain-english-1h38
https://mintlify.wiki/ibon-ira/Minishell/guide/redirections
https://mintlify.wiki/ibon-ira/Minishell/reference/architecture