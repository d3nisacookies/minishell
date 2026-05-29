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

### 4. Parsing Words, Quotes and Redirections

Once pipes have been separated, each individual command in the pipeline is passed to `parse_single()`.

For example, from the pipeline:

```bash
cat file.txt | grep "hello world" > result.txt
```

the second command reaches `parse_single()` as:

```bash
grep "hello world" > result.txt
```

At this stage, Minishell needs to identify the command arguments and any redirections attached to that command.

#### Parsing Individual Words

`parse_single()` first calls `parser_count_args()` to determine how much memory is needed for the command. It then creates a `t_cmd` structure and uses `fill_args()` to move through the input one word at a time.

Inside `fill_args()`, `parser_extract_word()` extracts each meaningful piece of the command.

For example:

```bash
grep "hello world" > result.txt
```

is read as:

```text
"grep"
"hello world"
">"
"result.txt"
```

Normal words are stored in the command's `args` array. Therefore, the executable arguments become:

```c
cmd->args[0] = "grep";
cmd->args[1] = "hello world";
cmd->args[2] = NULL;
```

#### Handling Quotes

Quotes allow text containing spaces or special characters to remain part of a single argument.

For example:

```bash
echo "hello world"
```

must not be parsed as:

```text
"echo"
"hello"
"world"
```

Instead, the quoted text is treated as one argument:

```text
"echo"
"hello world"
```

When `parser_extract_word()` finds single quotes (`'`) or double quotes (`"`), it continues reading until it finds the matching closing quote. The quote characters themselves are not stored as part of the final word. The parser also records that the argument originally contained quotes using the `quoted` array in the command structure.

Conceptually:

```bash
echo "hello world"
```

becomes:

```c
cmd->args[0] = "echo";
cmd->quoted[0] = 0;

cmd->args[1] = "hello world";
cmd->quoted[1] = 1;
```

If an opening quote does not have a matching closing quote, parsing fails and Minishell reports a syntax error instead of trying to execute an incomplete command.

#### Identifying Redirections

Redirection operators are also recognised while parsing the command. In the current implementation, the supported redirection operators are:

```text
<     input redirection
>     output redirection
>>    output redirection in append mode
```

The word parser recognises redirection characters separately from normal words. This means redirections can be recognised even when there are no spaces around them.

For example:

```bash
echo hello>result.txt
```

can be separated as:

```text
"echo"
"hello"
">"
"result.txt"
```

When `fill_args()` extracts a word, it checks whether that word is a redirection operator using `parser_is_redirection()`.

If it is a normal argument, it is placed inside `cmd->args`.

If it is a redirection operator, it is passed to `parser_set_redirection()`. This function extracts the following word as the filename and stores it in the appropriate field of the `t_cmd` structure.

For example:

```bash
grep "hello world" < input.txt > result.txt
```

is represented conceptually as:

```c
cmd->args[0] = "grep";
cmd->args[1] = "hello world";
cmd->args[2] = NULL;

cmd->infile = "input.txt";
cmd->outfile = "result.txt";
cmd->append = 0;
```

For append redirection:

```bash
echo hello >> log.txt
```

the command structure becomes:

```c
cmd->args[0] = "echo";
cmd->args[1] = "hello";
cmd->args[2] = NULL;

cmd->outfile = "log.txt";
cmd->append = 1;
```

The redirection symbols and their filenames are not stored as executable arguments. For example, in:

```bash
echo hello > result.txt
```

`echo` should receive only:

```c
["echo", "hello", NULL]
```

It should not receive:

```c
["echo", "hello", ">", "result.txt", NULL]
```

This is important because `>` is an instruction for Minishell, not an argument that should be passed to the `echo` command.

#### Parsing Does Not Open Files Yet

At this stage, Minishell only records the redirection information inside the `t_cmd` structure. It does not open `result.txt` or change standard input and output during parsing.

For example:

```bash
grep hello < input.txt > result.txt
```

is parsed into:

```text
Command:       grep
Argument:      hello
Input file:    input.txt
Output file:   result.txt
Append mode:   false
```

The actual file opening and `dup2()` operations happen later during execution, when the executor applies the stored redirections before running the command.

#### Resulting Command Structure Example

For the full pipeline:

```bash
cat input.txt | grep "hello world" >> result.txt
```

the completed parsed structure is conceptually:

```text
t_cmd 1:
    args = ["cat", "input.txt", NULL]
    infile = NULL
    outfile = NULL
    next ───────────────────────────┐
                                    ↓
t_cmd 2:
    args = ["grep", "hello world", NULL]
    infile = NULL
    outfile = "result.txt"
    append = 1
    next = NULL
```

At the end of this stage, Minishell has a linked list of command structures containing the arguments, quote information and redirections needed for execution.





### 5. Deciding Between a Builtin and an External Command

Once a command has been parsed into a `t_cmd` structure, Minishell passes it to `execute_command()`.

At this point, the command structure already contains the information needed for execution, such as:

```text
args        command name and arguments
infile      input redirection file, if present
outfile     output redirection file, if present
append      whether output should use >> append mode
next        the next command in a pipeline, if present
```

For example:

```bash
echo hello > result.txt
```

has already been parsed conceptually into:

```c
cmd->args[0] = "echo";
cmd->args[1] = "hello";
cmd->args[2] = NULL;

cmd->outfile = "result.txt";
cmd->append = 0;
cmd->next = NULL;
```

The executor must now decide how this command should be run.

#### Checking for a Pipeline First

The first decision is whether the command belongs to a pipeline.

```c
if (cmd->next)
{
	execute_pipeline(cmd, shell);
	return ;
}
```

If `cmd->next` exists, the current command is connected to at least one other command through a pipe.

For example:

```bash
cat file.txt | grep hello
```

has been parsed into linked command structures:

```text
cat file.txt  →  grep hello
```

Because the first `t_cmd` points to another command, `execute_command()` sends the linked list to `execute_pipeline()` instead of executing it as a single command.

If `cmd->next` is `NULL`, Minishell is dealing with one standalone command and can now decide whether it is a builtin or an external command.

#### Builtin Commands

A builtin is a command implemented directly inside Minishell rather than loaded from another executable file on the system.

In this implementation, standalone builtins include:

```text
exit
echo
cd
pwd
export
unset
```

Some builtins must be executed inside the parent shell process because they change the state of the shell itself.

For example:

```bash
cd Documents
```

must change Minishell's current working directory. If `cd` were executed only inside a child process, the child would move into `Documents`, then exit, while Minishell would remain in its original directory.

Similarly:

```bash
export USERNAME=jack
```

and:

```bash
unset USERNAME
```

must update the environment stored by the running shell so that later commands can use the new environment.

The `exit` builtin must also run in the shell process, because its purpose is to terminate Minishell itself.

Other builtins, such as `echo` and `pwd`, do not usually change permanent shell state. However, they are still implemented internally as part of Minishell's supported builtin behaviour.

#### Builtins With Redirections

A builtin can still use redirections.

For example:

```bash
echo hello > result.txt
```

does not run an external `echo` program. Instead, Minishell runs its own `builtin_echo()` function.

However, because the builtin is running inside the shell process, temporarily redirecting standard output also temporarily redirects Minishell's own output.

The execution flow is:

```text
Save the shell's original stdin and stdout
        ↓
Apply the stored redirection
        ↓
Run the builtin command
        ↓
Restore the original stdin and stdout
```

Conceptually:

```bash
echo hello > result.txt
```

causes `hello` to be written into `result.txt`, but after the command finishes, the shell prompt and later command output must still appear in the terminal.

Without restoring standard output, later output from Minishell could continue being written into `result.txt`.

#### External Commands

If the command is not recognised as a builtin, Minishell treats it as an external command.

Examples include:

```bash
ls
cat file.txt
grep hello result.txt
wc -l file.txt
```

External commands are separate executable programs installed on the system. Minishell does not contain the internal code for these programs. Instead, it creates a child process using `fork()`.

The child process then:

```text
Resets its command signals
        ↓
Applies any input or output redirections
        ↓
Runs the requested program using execvp()
```

For example:

```bash
grep hello < input.txt > result.txt
```

is executed conceptually as:

```text
Parent Minishell process
        ↓ fork()
Child process
        ↓
Connect input.txt to standard input
        ↓
Connect result.txt to standard output
        ↓
Replace child with grep using execvp()
```

The parent shell does not become `grep`. It waits for the child to finish, stores the resulting exit status in `shell->last_exit`, and then remains available to display another prompt.

This separation is important because `execvp()` replaces the process that calls it. If Minishell ran an external command using `execvp()` directly inside the parent process, the shell would be replaced by that program and would no longer continue after the command ended.

#### Example: Builtin Versus External Execution

For a builtin:

```bash
cd Documents
```

the flow is:

```text
Parsed command
        ↓
execute_command()
        ↓
Recognised as builtin cd
        ↓
Run builtin_cd() inside Minishell
        ↓
Minishell's directory is permanently changed
```

For an external command:

```bash
ls -l
```

the flow is:

```text
Parsed command
        ↓
execute_command()
        ↓
Not recognised as a builtin
        ↓
fork() creates a child process
        ↓
Child runs ls using execvp()
        ↓
Parent Minishell waits and continues afterwards
```

At the end of this stage, Minishell has chosen the correct execution method: pipelines are handed to pipeline execution, builtins are executed internally, and external commands are run through a child process.

### 6. Executing Commands

After parsing has finished, Minishell has a structured description of the user's command stored inside one or more `t_cmd` structures. These structures contain the command arguments, any redirections, and pointers connecting commands in a pipeline.

Execution begins in `execute_command()`.

#### Executing a Standalone Builtin

If the command is a builtin and is not part of a pipeline, Minishell executes its own internal function directly.

For example:

```bash
echo hello > result.txt
```

has already been parsed into information similar to:

```text
Command:        echo
Arguments:      hello
Output file:    result.txt
Append mode:    false
Pipeline next:  NULL
```

Because `echo` is a builtin, Minishell does not launch another executable program. Instead, it runs `builtin_echo()` inside the shell process.

However, the redirection must still be applied. Before running the builtin, Minishell saves its original standard input and standard output using `dup()`. It then calls `apply_redirections()` so that the builtin writes to the requested file.

Conceptually:

```text
Save original stdin and stdout
        ↓
Redirect stdout to result.txt
        ↓
Run builtin_echo()
        ↓
Restore original stdin and stdout
```

This allows:

```bash
echo hello > result.txt
```

to write `hello` into `result.txt`, while ensuring that the next shell prompt and future command output still appear in the terminal.

The same redirection-saving approach is used for standalone builtins such as `cd`, `pwd`, `export`, and `unset`.

#### Applying Redirections

The parser only records redirection information. The executor is the stage that actually opens files and changes where a command reads from or writes to.

`apply_redirections()` checks the redirection fields stored in the `t_cmd` structure.

For input redirection:

```bash
grep hello < input.txt
```

Minishell opens `input.txt` in read-only mode and uses `dup2()` to make it the command's standard input.

```text
input.txt → standard input → grep
```

For output redirection:

```bash
echo hello > result.txt
```

Minishell opens `result.txt` in output mode, creating it if necessary and truncating its previous contents. It then uses `dup2()` to make the file the command's standard output.

```text
echo → standard output → result.txt
```

For append redirection:

```bash
echo hello >> log.txt
```

Minishell opens `log.txt` in append mode, so the new output is added to the end of the file rather than replacing its existing contents.

#### Executing a Standalone External Command

If the command is not recognised as a builtin and is not part of a pipeline, Minishell treats it as an external command.

For example:

```bash
grep hello < input.txt > result.txt
```

is executed by creating a child process with `fork()`.

The parent process remains as Minishell. The child process prepares itself to become the requested command:

```text
Minishell parent process
        ↓
fork()
        ↓
Child process
        ↓
Reset child signal behaviour
        ↓
Apply stored redirections
        ↓
Run the external program using execvp()
```

In this example, the child first redirects its standard input from `input.txt` and its standard output into `result.txt`. It then calls:

```c
execvp(cmd->args[0], cmd->args);
```

`execvp()` replaces the child process with the requested external program. The parent shell is not replaced, so it can continue running after the external command finishes.

The parent waits for the child process using `waitpid()`. Once the command completes, Minishell stores its exit status in `shell->last_exit`.

If the command exits normally, Minishell stores its normal exit code. If the command is terminated by a signal, Minishell stores the signal-based status value.

This stored status is later used for behaviour such as:

```bash
echo $?
```

#### Executing a Pipeline

If the first `t_cmd` has a non-`NULL` `next` pointer, the command is part of a pipeline and `execute_command()` sends it to `execute_pipeline()`.

For example:

```bash
cat input.txt | grep hello > result.txt
```

was parsed into linked command structures similar to:

```text
cat input.txt  →  grep hello > result.txt
```

The executor must now connect these commands so that the output of `cat` becomes the input of `grep`.

For each command in the linked list, `execute_pipeline()` creates a child process. When a command has another command after it, a pipe is created using `pipe()`.

A pipe has two ends:

```text
pipefd[0] = read end
pipefd[1] = write end
```

For the example:

```bash
cat input.txt | grep hello > result.txt
```

the connection is:

```text
cat input.txt
        ↓ standard output
     pipe write end
        ↓
     pipe read end
        ↓ standard input
grep hello
        ↓ standard output redirected
result.txt
```

The first child process runs `cat input.txt`. Because another command follows it, its standard output is connected to the write end of the pipe using `dup2()`.

The second child process runs `grep hello`. Its standard input is connected to the read end of the previous pipe. Since this command also has `> result.txt`, its standard output is redirected into `result.txt`.

Conceptually:

```text
Child 1:
    stdin  = terminal or stored input redirection
    stdout = pipe write end
    runs cat input.txt

Child 2:
    stdin  = pipe read end
    stdout = result.txt
    runs grep hello
```

Each child closes file descriptors that it no longer needs after the correct input and output streams have been connected. The parent also closes pipe ends once they are no longer required, preventing unused file descriptors from keeping pipes open.

After creating all of the pipeline children, the parent process waits for them to finish. Minishell stores the exit status of the final command in the pipeline as `shell->last_exit`.

For example, in:

```bash
cat input.txt | grep hello > result.txt
```

the final pipeline status comes from `grep`, because it is the last command in the chain.

#### Full Execution Example

Consider the complete input:

```bash
echo start ; cat input.txt | grep "hello world" >> result.txt
```

Earlier parsing stages produce two separate command sequences:

```text
Sequence 1:
    echo start

Sequence 2:
    cat input.txt  →  grep "hello world" >> result.txt
```

Execution then happens in order.

First sequence:

```text
Run builtin_echo()
        ↓
Print "start" to the terminal
```

Second sequence:

```text
Create a pipe
        ↓
Fork child process for cat
        ↓
Connect cat stdout to pipe write end
        ↓
Fork child process for grep
        ↓
Connect grep stdin to pipe read end
        ↓
Open result.txt in append mode
        ↓
Connect grep stdout to result.txt
        ↓
Run both commands
        ↓
Wait for pipeline children
        ↓
Store the exit status of grep
```

At the end of execution, the shell remains alive and is ready to display the next prompt.

#### Execution Flow Summary

```text
Parsed t_cmd structure
        ↓
execute_command()
        ↓
Is there a next command?
        ├── Yes → execute_pipeline()
        │           ├── Create pipes
        │           ├── Fork one child per command
        │           ├── Connect stdin/stdout with dup2()
        │           ├── Apply file redirections
        │           ├── Run programs with execvp()
        │           └── Wait and save final status
        │
        └── No → Is it a builtin?
                    ├── Yes → Save streams
                    │         Apply redirections
                    │         Run builtin
                    │         Restore streams
                    │
                    └── No → fork()
                              Apply redirections in child
                              Run external program with execvp()
                              Wait and save status
```


### 7. Handling Pipes and Redirections

Once Minishell has decided that a command is part of a pipeline or contains redirections, it must change where the command reads its input from and where it sends its output.

Normally, a command uses:

```text
Standard input  (stdin)  → keyboard / terminal input
Standard output (stdout) → terminal display
```

For example:

```bash
cat
```

normally reads text typed by the user and prints it back to the terminal.

Pipes and redirections change these normal connections.

---

#### File Descriptors

Unix programs use file descriptors to represent input and output streams.

The most important standard file descriptors are:

```text
STDIN_FILENO   = 0    standard input
STDOUT_FILENO  = 1    standard output
STDERR_FILENO  = 2    standard error
```

Minishell uses `dup2()` to replace standard input or standard output with a file or a pipe connection.

For example:

```c
dup2(fd, STDOUT_FILENO);
```

means:

```text
Make standard output write to fd instead of the terminal.
```

After this, anything the command prints to standard output is sent through `fd`.

---

#### Output Redirection: `>`

The output redirection operator `>` sends a command's output into a file instead of displaying it in the terminal.

For example:

```bash
echo hello > result.txt
```

normally means:

```text
echo output → result.txt
```

To do this, Minishell opens `result.txt` with write permissions. If the file does not exist, it is created. If it already contains text, its previous contents are removed because `>` uses truncate mode.

Conceptually:

```c
fd = open("result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

After standard output has been redirected, running:

```bash
echo hello
```

writes:

```text
hello
```

into `result.txt` rather than printing it in the terminal.

---

#### Append Redirection: `>>`

The append operator `>>` also sends output into a file, but it preserves any existing content and adds the new output to the end.

For example:

```bash
echo first > log.txt
echo second >> log.txt
```

produces:

```text
first
second
```

inside `log.txt`.

The difference is in how the file is opened:

```c
fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

Therefore:

```text
>     replaces previous file contents
>>    adds new output to the end of the file
```

---

#### Input Redirection: `<`

The input redirection operator `<` causes a command to read from a file instead of waiting for input from the terminal.

For example:

```bash
grep hello < input.txt
```

means:

```text
input.txt → grep
```

Minishell opens the file in read-only mode and connects it to standard input:

```c
fd = open("input.txt", O_RDONLY);
dup2(fd, STDIN_FILENO);
close(fd);
```

Now, when `grep` reads from standard input, it receives the contents of `input.txt`.

Without redirection:

```bash
grep hello
```

would wait for the user to type input.

With redirection:

```bash
grep hello < input.txt
```

it automatically searches the contents of `input.txt`.

---

#### Pipes: `|`

A pipe connects the standard output of one command to the standard input of another command.

For example:

```bash
cat input.txt | grep hello
```

means:

```text
cat reads input.txt
        ↓
cat output is sent into a pipe
        ↓
grep reads from the pipe
        ↓
grep prints matching lines
```

Minishell creates a pipe using:

```c
pipe(pipefd);
```

A pipe gives two file descriptors:

```text
pipefd[0]    read end of the pipe
pipefd[1]    write end of the pipe
```

The first command writes into the pipe:

```c
dup2(pipefd[1], STDOUT_FILENO);
```

The next command reads from the pipe:

```c
dup2(pipefd[0], STDIN_FILENO);
```

Therefore, for:

```bash
cat input.txt | grep hello
```

the connection is:

```text
Child process running cat:
    stdout → pipefd[1]

Child process running grep:
    stdin  ← pipefd[0]
```

Conceptually:

```text
cat input.txt  →  pipe  →  grep hello  →  terminal
```

---

#### Pipes Combined With Redirections

Pipes and redirections can be used together.

For example:

```bash
cat input.txt | grep hello > result.txt
```

contains both a pipe and an output redirection.

The required connections are:

```text
cat input.txt  →  pipe  →  grep hello  →  result.txt
```

For the first command:

```text
cat
    stdin  = terminal, unless redirected
    stdout = write end of pipe
```

For the second command:

```text
grep
    stdin  = read end of pipe
    stdout = result.txt
```

Conceptually:

```text
Child 1: cat input.txt
    dup2(pipefd[1], STDOUT_FILENO)
    execvp("cat", ...)

Child 2: grep hello > result.txt
    dup2(pipefd[0], STDIN_FILENO)
    apply output redirection to result.txt
    execvp("grep", ...)
```

The final output does not appear in the terminal because the last command's output has been redirected into `result.txt`.

---

#### Multiple Pipes

A pipeline may contain more than two commands.

For example:

```bash
cat input.txt | grep hello | wc -l > count.txt
```

represents:

```text
cat input.txt  →  grep hello  →  wc -l  →  count.txt
```

Each command receives input from the previous command and sends output to the next command, except:

```text
The first command reads from its normal input or an input redirection.
The last command writes to its normal output or an output redirection.
```

Conceptually:

```text
Child 1: cat input.txt
    stdout → Pipe 1 write end

Child 2: grep hello
    stdin  ← Pipe 1 read end
    stdout → Pipe 2 write end

Child 3: wc -l > count.txt
    stdin  ← Pipe 2 read end
    stdout → count.txt
```

This allows Minishell to build a chain of commands where each program performs one part of a larger task.

---

#### Closing Unused File Descriptors

After using `dup2()` to connect input and output correctly, each process must close file descriptors it no longer needs.

For example, after the child running `cat` connects its standard output to the write end of the pipe, it no longer needs the original pipe descriptors:

```c
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[0]);
close(pipefd[1]);
```

The parent process must also close old pipe ends after creating each child.

This is important because leaving unused pipe descriptors open can cause commands to wait forever for input. A command reading from a pipe only receives end-of-file once every write end of that pipe has been closed.

For example, in:

```bash
cat input.txt | grep hello
```

`grep` must eventually know that `cat` has finished writing. If another unused copy of the pipe's write end remains open in the parent, `grep` may continue waiting instead of finishing.

---

#### Restoring Redirections for Builtins

External commands and pipeline commands run in child processes. When a child redirects its input or output, those changes disappear when the child exits.

Standalone builtins are different because they run directly inside Minishell.

For example:

```bash
echo hello > result.txt
```

temporarily redirects the shell's own standard output while its internal `echo` function runs.

Minishell must therefore:

```text
Save its original standard input and output
        ↓
Apply the redirection
        ↓
Run the builtin
        ↓
Restore the original standard input and output
```

Without restoring standard output, the shell prompt and later output could continue being written into `result.txt` rather than appearing in the terminal.

---

#### Full Example

Consider:

```bash
echo start ; cat input.txt | grep "hello world" >> result.txt
```

The first command sequence is:

```bash
echo start
```

Since there is no redirection, `echo` prints:

```text
start
```

to the terminal.

The second command sequence is:

```bash
cat input.txt | grep "hello world" >> result.txt
```

This is executed as:

```text
Open a pipe
        ↓
Fork child for cat
        ↓
Connect cat stdout to the pipe write end
        ↓
Fork child for grep
        ↓
Connect grep stdin to the pipe read end
        ↓
Open result.txt in append mode
        ↓
Connect grep stdout to result.txt
        ↓
Run both commands
        ↓
Close unused file descriptors
        ↓
Wait for the child processes to finish
```

The final data flow is:

```text
input.txt
    ↓
cat
    ↓
pipe
    ↓
grep "hello world"
    ↓
result.txt, in append mode
```

At the end, Minishell restores or retains its own normal terminal input and output, stores the exit status of the final command, and displays the next prompt.

---

#### Handling Pipes and Redirections Summary

```text
<     replaces standard input with a file
>     replaces standard output with a new or truncated file
>>    replaces standard output with a file opened in append mode
|     connects one command's standard output to the next command's standard input
```

In Minishell, parsing records that these operators exist. Execution is the stage that uses `open()`, `pipe()`, `dup2()` and `close()` to build the actual input and output connections required by the command.

### 8. Updating `$?`

In Bash, `$?` stores the exit status of the most recently executed command. Minishell keeps track of this value using `shell->last_exit`.

An exit status tells the shell whether a command succeeded or failed.

By convention:

```text
0       success
non-zero error / failure
```

For example:

```bash
echo hello
echo $?
```

would print:

```text
hello
0
```

because `echo hello` completed successfully.

If a command fails:

```bash
ls does_not_exist
echo $?
```

Bash returns a non-zero value, usually `2` for this kind of `ls` error.

---

#### Updating `$?` After Builtins

When Minishell runs a builtin, it updates `shell->last_exit` depending on whether the builtin succeeded or failed.

For example:

```bash
pwd
```

should set:

```c
shell->last_exit = 0;
```

if the command succeeds.

For a failing command such as:

```bash
cd folder_that_does_not_exist
```

`cd` should set:

```c
shell->last_exit = 1;
```

because the directory change failed.

This allows the user to check the result afterwards:

```bash
cd folder_that_does_not_exist
echo $?
```

Conceptually:

```text
cd fails
    ↓
shell->last_exit = 1
    ↓
echo $? prints 1
```

---

#### Updating `$?` After External Commands

External commands are executed in a child process. The parent shell waits for the child using `waitpid()`.

When the child finishes, Minishell checks how it ended.

If the child exited normally, Minishell stores the child's exit code:

```c
if (WIFEXITED(status))
	shell->last_exit = WEXITSTATUS(status);
```

For example:

```bash
grep hello file.txt
echo $?
```

If `grep` found a match, its exit status is `0`.

If `grep` found no match, its exit status is `1`.

If there was an actual error, such as the file not existing, the status is usually `2`.

Minishell stores this value in `shell->last_exit`, so `$?` can expand to the correct result later.

---

#### Updating `$?` After Signals

A command may also end because it receives a signal.

For example, if a running command is interrupted with `Ctrl-C`, it receives `SIGINT`.

In this case, Bash-style shells usually store:

```text
128 + signal_number
```

For `SIGINT`, the signal number is `2`, so the exit status becomes:

```text
128 + 2 = 130
```

This is why pressing `Ctrl-C` often makes `$?` become:

```text
130
```

In code, this is handled with:

```c
else if (WIFSIGNALED(status))
	shell->last_exit = 128 + WTERMSIG(status);
```

Conceptually:

```text
Command interrupted by Ctrl-C
        ↓
Signal = SIGINT
        ↓
Signal number = 2
        ↓
shell->last_exit = 130
```

---

#### Updating `$?` After Pipelines

For a pipeline, the final exit status usually comes from the **last command in the pipeline**.

For example:

```bash
cat file.txt | grep hello
echo $?
```

The value of `$?` should come from `grep hello`, because `grep` is the final command in the pipeline.

Conceptually:

```text
cat file.txt  →  grep hello
                    ↓
              final pipeline command
                    ↓
              shell->last_exit
```

So if `cat` succeeds but `grep` finds no match, `$?` becomes `1`, because the final command was `grep`.

For another example:

```bash
cat missing_file | grep hello
```

Even though `cat` fails because the file does not exist, the final status may still depend on how `grep` exits, because `grep` is the last command in the pipeline.

This matches normal shell behaviour unless extra options such as `pipefail` are used, which Minishell does not need to implement.

---

#### Updating `$?` After Parsing or Syntax Errors

`$?` must also be updated when Minishell detects a syntax error before execution.

For example:

```bash
echo hello >
```

is invalid because the redirection operator `>` is missing a filename.

In this case, Minishell should not execute the command. Instead, it reports a syntax error and updates the exit status.

Commonly, shell syntax errors use exit status:

```text
2
```

Conceptually:

```text
Parser finds invalid syntax
        ↓
Command is not executed
        ↓
shell->last_exit = 2
```

This means:

```bash
echo hello >
echo $?
```

should show the syntax error status rather than the result of a command execution.

---

#### `$?` During Expansion

When the user types:

```bash
echo $?
```

Minishell expands `$?` into the current value of `shell->last_exit`.

For example, if the previous command failed and set:

```c
shell->last_exit = 1;
```

then:

```bash
echo $?
```

becomes:

```bash
echo 1
```

before execution.

So `$?` is not calculated by `echo`. It is expanded by Minishell before the command runs.

---

#### Example Flow

Consider this sequence:

```bash
pwd
echo $?
cd missing_folder
echo $?
```

The flow is:

```text
Run pwd
    ↓
pwd succeeds
    ↓
shell->last_exit = 0
    ↓
echo $? expands to echo 0

Run cd missing_folder
    ↓
cd fails
    ↓
shell->last_exit = 1
    ↓
echo $? expands to echo 1
```

So the output would be conceptually:

```text
/current/directory
0
cd: missing_folder: No such file or directory
1
```

---

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
