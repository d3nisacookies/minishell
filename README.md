# Minishell

A small Unix shell written in C (42 curriculum project).

Minishell implements a focused, mandatory subset of a Bash-like interactive
shell: it reads commands, parses quotes and operators, executes builtins and
external programs, handles pipes and redirections, expands environment variables,
and preserves shell state between commands.

This repository is intended to illustrate core shell primitives: process
creation, file descriptor control, environment management, signal handling,
parsing, and error handling.

## Table of Contents

- [Features](#features)
- [Build and run](#build-and-run)
- [Quick usage examples](#quick-usage-examples)
- [Project structure](#project-structure)
- [How it works (high level)](#how-it-works-high-level)
- [Testing](#testing)
- [Memory checks](#memory-checks)
- [Known scope](#known-scope)
- [Authors](#authors)

## Features

- Interactive prompt using `readline` and command history
- External command execution using `fork()` + `execve()`
- PATH resolution for external commands
- Builtins: `echo`, `cd`, `pwd`, `export`, `unset`, `exit`
- Environment variable management and expansion (including `$?`)
- Proper handling of single and double quotes
- Pipelines using `|`
- Redirections: `<`, `>`, `>>`, and heredoc `<<`
- Semicolon-separated command sequences
- Exit status tracking (`shell->last_exit` used for `$?`)
- Signal handling tuned for the interactive shell, child processes, and heredoc
- `Ctrl-D` (EOF) handling to exit the shell

## Build and run

Requirements

- `gcc`
- `make` (GNU make)
- `libreadline` development headers (`libreadline-dev` on Debian/Ubuntu)

On Debian/Ubuntu:

```bash
sudo apt install build-essential libreadline-dev
```

Build:

```bash
make
```

Run:

```bash
./minishell
```

Useful make targets:

- `make clean`  — remove object files
- `make fclean` — remove object files, libraries and the executable
- `make re`     — rebuild from scratch

## Quick usage examples

Basic commands:

```bash
pwd
echo hello world
ls -la
```

Builtins and environment variables:

```bash
cd ..
pwd
export NAME=minishell
echo "$NAME"
unset NAME
```

Pipes and redirections:

```bash
ls -la | grep ".c"
echo hello > file.txt
cat < file.txt
echo again >> file.txt
```

Heredoc example:

```bash
cat << EOF
hello from heredoc
EOF
```

Exit status and quit:

```bash
ls missing_file
echo $?
exit
```

You can also press `Ctrl-D` on an empty prompt to exit.

## Project structure

Key directories and files:

```text
.
├── builtins/       # builtin implementations (cd, echo, export, unset, pwd, exit)
├── env/            # environment and exported variable helpers
├── executor/       # execution, pipes, PATH lookup, child handling
├── parser/         # tokenization, syntax checks, word parsing, redirections
├── redirections/   # redirection helpers and heredoc handling
├── printf/         # local ft_printf and libft dependency used in project
├── minishell.h     # shared types and function declarations
├── main.c          # program entrypoint
├── prompt.c        # interactive prompt loop and readline integration
├── signals.c       # signal handling for different execution modes
└── Makefile
```

See the source directories for more details and comments on behaviour.

## How it works (high level)

1. Read a line from the user using `readline()`.
2. Add non-empty input to history and pass the line to the parser.
3. Parser validates syntax and converts input into a linked list of commands
  (`t_cmd` nodes), splitting on semicolons and pipes and extracting words while
  respecting quotes. Redirections are recorded separately from argument lists.
4. The executor walks the command list, deciding whether each command runs as a
  builtin (executed in the parent for state-changing builtins when appropriate)
  or as an external program (spawned in a child process).
5. For pipelines, appropriate pipes are created and `dup2()` is used to wire
  stdin/stdout for each child process; unused descriptors are closed.
6. Redirections are applied just before command execution. Builtins running in
  the parent temporarily replace file descriptors and restore them afterwards.

The shell tracks the most recent exit status in `shell->last_exit` and uses it
for `$?` expansion.

## Data Structure Choices

Before settling on the current parser model, we considered a few ways to
represent user input after tokenization. This section explains the tradeoffs
we evaluated and why the current implementation uses a linked-list of commands.

### AST: Abstract Syntax Tree

An Abstract Syntax Tree represents commands by their logical relationships
rather than only by their order in the input string. An AST-based parser asks
questions such as: what is the main operator, which tokens belong on the left
or right, and what has higher precedence? For a larger shell this gives a clear
structure for nested expressions, logical operators, and precedence rules. The
downside is added complexity: building and traversing an AST requires more
parsing logic than the Minishell mandatory scope needs.

### CST: Concrete Syntax Tree

A Concrete Syntax Tree keeps a more literal representation of the input. It
preserves more grammar details and intermediate nodes, which can simplify
validation because the tree closely mirrors the grammar. The tradeoff is that a
CST can become verbose, and the executor must navigate extra layers to reach
useful command/argument/redirection information.

### Linked list (chosen)

We chose a linked list of commands (`t_cmd` nodes). Each node represents one
command with its arguments, quote information, and a redirection list. Pipes are
represented by the `next` pointer connecting command nodes. This keeps the
parser and executor straightforward for the required features and maps naturally
to left-to-right execution of pipelines. The limitation is that a linked list
doesn't express more advanced grammar (e.g., `&&`, `||`, or grouped expressions)
without additional structure.

## Testing

This repository includes a `minishell_tester/` helper used in the 42 community.
From inside that directory:

```bash
./tester
```

Run specific test groups, for example:

```bash
./tester builtins
./tester pipes
./tester redirects
./tester syntax
```

Manual test suites are available under `minishell_tester/manual_tests/`.

## Memory checks

When running Valgrind use the provided suppression file to ignore readline's
internal allocations:

```bash
valgrind --leak-check=full --show-leak-kinds=all \
  --suppressions=valgrind_readline.supp ./minishell
```

Note: `readline` may keep reachable memory until process exit; the suppression
file helps focus the report on Minishell's allocations.

## Known scope

This project targets the mandatory 42 Minishell requirements. It is not a full
Bash clone and intentionally omits advanced features such as `&&` / `||`,
subshells, command substitution, globbing, and job control.

## Authors

- jculleto
- akaung

