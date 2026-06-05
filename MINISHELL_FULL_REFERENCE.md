# Minishell Full Reference and Evaluation Revision Book

This document is a deep revision guide for the core minishell implementation in this repository.

Scope of this document:
- All core minishell source files in root, builtins, env, parser, executor, redirections.
- Excludes ft_printf and libft internals in printf, because those are external utility libraries used by minishell.

Goal:
- Help you understand full workflow from prompt to cleanup.
- Explain every function in core minishell with practical behavior notes.
- Prepare you for evaluation questions.

## 1) End to End Workflow

High-level runtime loop:
1. main initializes shell state and signal handling.
2. prompt loop reads one line.
3. line is split by semicolons while respecting quotes.
4. each segment is validated and parsed into one command list (pipeline chain).
5. executor expands arguments and runs builtin, external command, or pipeline.
6. shell exit code is updated from result.
7. allocated command structures are freed.
8. loop continues until exit condition or EOF.

Execution decision tree for one parsed command chain:
1. if command list is empty: nothing to do.
2. if first command expands to no args: set success and return.
3. if chain has next pointer: run pipeline path.
4. else if command is builtin: run builtin path.
5. else run external fork and exec path.

Error propagation:
- parser writes parser_status in shell.
- prompt and executor copy relevant status into shell.last_exit.
- shell.last_exit is used for dollar question mark expansion and final program return.

## 2) Core Data Structures

### t_shell
Fields:
- env: active environment array.
- last_exit: most recent command status.
- should_exit: set by exit builtin.
- parser_status: parser-specific status store.

Why it matters:
- It is the shared state across parser, executor, and builtins.
- Replaces many globals and centralizes behavior decisions.

### t_cmd
Represents one command node in a pipeline.
Fields include:
- args and quoted arrays.
- argc count.
- redirs linked list.
- legacy redirection fields infile outfile append heredoc heredoc_delim.
- next pointer to next pipeline stage.

### t_redir
Linked list node for one redirection operation.
- type: input output append heredoc.
- target: filename or heredoc delimiter.
- next: next redirection.

### t_pipe_exec
State holder used while executing pipelines.
- cmd current node.
- pids array for all children.
- idx index for pids insertion.
- fd_in read end from previous pipe.
- last_pid to determine final pipeline status.

## 3) Function by Function Reference

## main.c

### free_env_copy
Purpose:
- Frees duplicated environment array.
Inputs:
- env array.
Output:
- none.
Side effects:
- releases heap memory.
Edge notes:
- null-safe loop style expected.

### signal_handler
Purpose:
- handles SIGINT and SIGQUIT while shell waits at prompt.
Inputs:
- signum.
Output:
- none.
Side effects:
- updates global signal marker and readline display state.
Important behavior:
- SIGINT refreshes prompt line.
- SIGQUIT usually ignored in prompt mode.

### setup_signals
Purpose:
- installs process-level signal handlers for interactive shell behavior.
Inputs:
- none.
Output:
- none.
Side effects:
- alters signal dispositions.
Important behavior:
- readline internal signal catching disabled so shell controls redraw.

### main
Purpose:
- bootstraps shell and enters prompt loop.
Inputs:
- argc argv envp.
Output:
- returns shell.last_exit.
Side effects:
- allocates env copy and launches full shell lifecycle.
Important behavior:
- cleans readline history and env before exit.

## prompt.c

### check_segment
Purpose:
- validates one semicolon segment for leading or trailing pipe errors.
Inputs:
- segment array and index and shell.
Output:
- status indicator.
Side effects:
- sets parser status on syntax errors.

### validate_segments
Purpose:
- loops all semicolon segments and runs structural checks.
Inputs:
- segment array and shell.
Output:
- success or failure.
Side effects:
- may set parser status.

### run_segments
Purpose:
- parse and execute each segment in sequence.
Inputs:
- segment array and shell.
Output:
- status.
Side effects:
- executes commands and updates shell.last_exit.
Important behavior:
- stops early if exit builtin sets should_exit.

### execute_input_segments
Purpose:
- split input by semicolons then validate and execute.
Inputs:
- input string and shell.
Output:
- status.
Side effects:
- controls parser and executor flow for one input line.

### prompt_loop
Purpose:
- core read execute loop.
Inputs:
- shell.
Output:
- none.
Side effects:
- reads from terminal repeatedly and executes commands.
Important behavior:
- handles EOF cleanly.
- adds history for non-empty input.

## prompt_utils.c

### trim_spaces
Purpose:
- removes leading and trailing whitespace boundaries for checks.
Inputs:
- mutable string.
Output:
- pointer to trimmed start.
Side effects:
- may write null terminator to cut trailing spaces.

### is_trailing_empty_segment
Purpose:
- checks if empty segment after split is legal trailing case.
Inputs:
- segment array and index.
Output:
- boolean integer.

### ends_with_pipe
Purpose:
- detects if a segment ends with pipe after trimming.
Inputs:
- segment string.
Output:
- boolean integer.

### starts_with_pipe
Purpose:
- detects if a segment starts with pipe after trimming.
Inputs:
- segment string.
Output:
- boolean integer.

### handle_parse_fail
Purpose:
- converts parser failure into shell.last_exit and frees split arrays.
Inputs:
- shell and segments.
Output:
- failure code.
Side effects:
- updates shell.last_exit.

## prompt_error_token.c

### syntax_error_token
Purpose:
- prints standardized syntax error token message.
Inputs:
- shell and token string.
Output:
- failure code.
Side effects:
- sets parser status to syntax failure value.

## parser/parser.c

### free_cmd_chain
Purpose:
- frees a chain of parsed commands on parse failure paths.
Inputs:
- cmd head.
Output:
- none.

### free_pipeline_rest
Purpose:
- frees remaining pipe split strings starting at index.
Inputs:
- pipeline array and start index.
Output:
- none.

### append_cmd
Purpose:
- helper that parses one pipe segment and appends to cmd list.
Inputs:
- list pointers, segment text, shell.
Output:
- status.
Side effects:
- allocates and links new t_cmd node.

### build_cmd_list
Purpose:
- builds full t_cmd linked list from pipe segments.
Inputs:
- split pipeline and shell.
Output:
- status.
Side effects:
- constructs chain used by executor.

### parse_command
Purpose:
- parser entrypoint for one semicolon segment.
Inputs:
- input segment and shell.
Output:
- command list or null.
Side effects:
- sets parser status on syntax or allocation problems.

## parser/parser_single.c

### free_partial_cmd
Purpose:
- rollback helper for partially filled command allocation.
Inputs:
- cmd and count of args built.
Output:
- none.

### handle_word
Purpose:
- routes extracted token to arg list or redirection parser.
Inputs:
- cmd input cursor shell.
Output:
- status.
Side effects:
- mutates cmd fields.

### fill_args
Purpose:
- scans command text and fills args plus redirections.
Inputs:
- cmd input shell.
Output:
- status.

### alloc_cmd
Purpose:
- allocates command structure and args arrays from arg count.
Inputs:
- argc.
Output:
- allocated cmd or null.

### parse_single
Purpose:
- parser for one non-pipe command segment.
Inputs:
- segment text and shell.
Output:
- t_cmd or null.
Important behavior:
- couples parser_count_args with fill_args.

## parser/parser_pipes.c

### count_pipes
Purpose:
- counts unquoted pipe separators.
Inputs:
- raw segment text.
Output:
- count or error indicator.

### push_segment
Purpose:
- extracts and stores one pipe-delimited segment.
Inputs:
- result array indexes input range.
Output:
- status.

### is_pipe_split
Purpose:
- quote-aware check for split point at current index.
Inputs:
- input index and quote state.
Output:
- boolean integer.

### split_loop
Purpose:
- main scan loop that fills pipeline segment array.
Inputs:
- input and output array.
Output:
- status.

### split_pipes
Purpose:
- public pipe splitting entrypoint.
Inputs:
- input and shell.
Output:
- null-terminated array of pipeline segments.
Side effects:
- sets parser status on quote errors.

## parser/parser_syntax.c

### parser_validate_syntax
Purpose:
- performs global syntax check focusing on quote balance.
Inputs:
- input shell.
Output:
- status.
Side effects:
- records parser errors in shell.

## parser/parser_utils.c

### parser_set_status
Purpose:
- writes parser status to shell.

### parser_get_status
Purpose:
- reads parser status from shell.

### parser_is_space
Purpose:
- whitespace predicate used throughout parser.

### parser_skip_spaces
Purpose:
- increments index over whitespace span.

### parser_free_split
Purpose:
- frees split array with known count.

## parser/parser_word_utils.c

### parser_is_redir_char
Purpose:
- predicate for redirection operators.

### parser_skip_quoted
Purpose:
- skip quoted range from index.
Output:
- closing index or error.

### parser_get_redir_len
Purpose:
- returns operator length one or two for redir tokens.

## parser/parser_words.c

### get_word_len
Purpose:
- computes allocation size for next token extraction.

### fill_redirection_word
Purpose:
- writes operator token into output buffer.

### copy_quoted_content
Purpose:
- copies quoted block contents without quote delimiters.

### fill_word
Purpose:
- builds final token text while tracking quote flag.

### parser_extract_word
Purpose:
- public token extractor for parser.
Inputs:
- input index pointer and quoted-output flag.
Output:
- allocated token string.
Important behavior:
- must not be passed a null quoted flag pointer when function expects to write it.

## parser/parser_count_args.c

### is_redir_char
Purpose:
- local predicate for count scan.

### skip_quoted
Purpose:
- index movement over quoted sequence during counting.

### skip_word_token
Purpose:
- skips one logical token boundary.

### parser_count_args
Purpose:
- counts non-space tokens for allocation planning.

## parser/parser_redir_check.c

### parser_is_redirection
Purpose:
- identifies exact redirection operator tokens.

## parser/parser_redirections.c

### append_redir
Purpose:
- append redirection node to command list.

### set_input_redir
Purpose:
- sync legacy input or heredoc fields.

### sync_legacy_redirection
Purpose:
- updates legacy command redirection fields from new list representation.

### parser_add_redirection
Purpose:
- allocates redirection node and attaches to command.

### parser_set_redirection
Purpose:
- parses operator plus target and stores redirection.
Inputs:
- cmd input cursor shell.
Output:
- status.
Important behavior:
- reports syntax error if missing target after operator.

## parser/parser_semicolon_count.c

### parser_count_semicolons
Purpose:
- counts unquoted semicolon separators.

### parser_redirection_error
Purpose:
- standard redirection syntax error reporting helper.

### get_redir_type
Purpose:
- converts token to redirection enum.

## parser/parser_semicolons.c

### quote_state_changed
Purpose:
- updates quote-state machine while scanning semicolons.

### append_segment
Purpose:
- stores allocated segment in output list.

### fill_loop
Purpose:
- scans raw input and cuts segments at valid semicolons.

### fill_segments
Purpose:
- orchestration for semicolon segment extraction.

### split_semicolons
Purpose:
- public semicolon splitter.
Inputs:
- raw line and shell.
Output:
- segment array.

## parser/parser_split_free.c

### free_split_array
Purpose:
- generic free helper for null-terminated string arrays.

## parser/parser_errors.c

### parser_put_unmatched_quote_error
Purpose:
- prints unmatched quote syntax message and sets parser status.

### parser_put_pipe_error
Purpose:
- prints pipe syntax message and sets parser status.

## executor/executor.c

### run_regular_builtin
Purpose:
- dispatches supported regular builtins.
Inputs:
- cmd shell.
Output:
- none.
Side effects:
- runs builtin and updates shell.last_exit.

### execute_builtin
Purpose:
- builtin gate with stdio save, redirection apply, and restore.
Inputs:
- cmd shell.
Output:
- handled flag.
Important behavior:
- exit builtin is handled specially.
- redirection failure sets exit status and returns handled.

### execute_external_child
Purpose:
- child setup for non-builtin execution.
Steps:
- reset SIGINT and SIGQUIT to default.
- apply redirections.
- resolve path.
- call execve.
- on failure dispatch error classifier.

### execute_external
Purpose:
- fork and wait wrapper for external commands.
Updates:
- shell.last_exit from child status.

### execute_command
Purpose:
- top-level executor dispatcher.
Order:
1. validate cmd.
2. expand args.
3. handle empty-after-expansion.
4. pipeline path if cmd.next exists.
5. builtin path.
6. external path.

## executor/executor_expand.c

### append_str
Purpose:
- dynamic append helper used during expansion construction.

### expand_dollar
Purpose:
- parses and expands dollar expressions.
Cases:
- dollar question mark maps to shell.last_exit.
- dollar name maps to env value.
- unmatched patterns produce empty or literal behavior depending on parser logic.

### expand_argument
Purpose:
- expands one argument string based on quote context.
Important behavior:
- single-quoted arguments remain literal.

### fill_expanded_args
Purpose:
- loops existing args and builds replaced arrays.
Important behavior:
- preserves quoted-empty arguments.
- can drop unquoted-empty results.

### executor_expand_args
Purpose:
- public argument expansion API.
Side effects:
- replaces cmd args memory with new expanded memory.

## executor/executor_path.c

### join_command_path
Purpose:
- creates dir slash cmd full path string.

### path_is_executable
Purpose:
- tests candidate path with execute access check.

### resolve_path_from_env
Purpose:
- splits PATH and searches executable candidate.
Output:
- full path or null.
Side effects:
- sets errno to ENOENT when not found.

## executor/executor_utils.c

### is_nonexec_file
Purpose:
- checks whether bare command refers to existing non-directory non-executable file.
Use case:
- used for differentiating command-not-found versus permission-denied style behavior.

### resolve_command_path
Purpose:
- command resolution wrapper.
Flow:
1. reject empty names.
2. if command has slash return as-is copy.
3. search PATH via resolve_path_from_env.
4. on not-found may consider nonexec file behavior.

### restore_stdio
Purpose:
- restores saved stdin and stdout fds after builtin execution.

## executor/executor_exec_error.c

### is_directory_path
Purpose:
- checks whether command path points to directory and includes slash semantics.

### executor_exit_exec_error
Purpose:
- maps exec failure context to message and exit code.
Mapping:
- directory path to 126.
- ENOENT to 127.
- EACCES to 126.
- fallback perror to 1.

## executor/executor_exit.c

### get_exit_sign
Purpose:
- handles sign and parse limits for numeric exit input.

### parse_exit_code
Purpose:
- strict numeric parser with overflow guards.
Output:
- parsed unsigned char code.

### builtin_exit
Purpose:
- implements exit builtin behavior and validation.
Behavior:
- no args uses current status.
- invalid numeric arg triggers status 2 and immediate exit intent.
- too many args keeps shell alive with status 1.

## executor/executor_pipe_init.c

### count_pipeline_cmds
Purpose:
- counts command nodes to size pid array.

### init_pipeline_exec
Purpose:
- allocates and initializes pipeline execution context struct.
Failure behavior:
- sets shell.last_exit on allocation failure.

## executor/executor_pipe.c

### close_pipe_pair
Purpose:
- closes both ends for newly created pipe when needed.

### run_pipeline_step
Purpose:
- executes one stage setup in parent:
- expand args.
- create pipe if next stage exists.
- fork child.
- parent stores pid and updates fd_in for next stage.

### execute_pipeline
Purpose:
- full pipeline orchestration loop.
Flow:
1. init context.
2. iterate command nodes and run steps.
3. close residual fd.
4. wait all children via wait_pipeline.
5. free pid array.

## executor/executor_pipe_utils.c

### close_pipeline_fds
Purpose:
- closes inherited fd_in and stage pipe fds inside child.

### run_pipeline_execve
Purpose:
- resolves command path then executes with execve.
Fallback:
- uses executor error exit helper.

### setup_pipeline_child
Purpose:
- performs dup2 wiring for child stdin and stdout based on pipeline position.

### wait_pipeline
Purpose:
- waits all child pids and captures only last command status into shell.last_exit.

### execute_pipeline_child
Purpose:
- complete child routine for one pipeline stage.
Order:
1. restore default signals.
2. setup pipe fds via dup2.
3. apply redirections for this command.
4. exec command.
Important behavior:
- per-command redirection can override pipe defaults.

## executor/executor_free.c

### free_redirections
Purpose:
- frees redirection linked list and target strings.

### free_cmd
Purpose:
- frees one command node and owned dynamic fields.

### free_cmd_list
Purpose:
- iterates and frees full command chain.

## builtins/cd_pwd.c

### set_shell_env_var
Purpose:
- helper to set one env key value pair through export path.

### get_env_value
Purpose:
- returns environment value pointer for key lookup.

### builtin_pwd
Purpose:
- prints current working directory.

### builtin_cd
Purpose:
- changes current directory and updates OLDPWD and PWD.

## builtins/cd_utils.c

### cd_get_target
Purpose:
- resolves cd target from args and HOME fallback.
Checks:
- too many args handling.

### cd_update_pwd
Purpose:
- updates OLDPWD and PWD entries after successful directory change.

## builtins/echo_utils.c

### is_var_start
Purpose:
- validates first char for env variable names.

### is_var_char
Purpose:
- validates continuation chars for variable names.

## builtins/echo.c

### expand_variable
Purpose:
- expands one dollar variable occurrence while echo prints.

### print_expansion
Purpose:
- decides between dollar question mark variable name or literal.

### print_echo_arg
Purpose:
- prints one echo argument with optional expansion behavior based on quote flag.

### builtin_echo
Purpose:
- echo implementation with optional no-newline flag.

## builtins/export.c

### is_valid_identifier
Purpose:
- checks export identifier syntax.

### print_export_error
Purpose:
- prints export invalid identifier message.

### print_export
Purpose:
- prints sorted-like declaration format of environment entries.

### builtin_export
Purpose:
- export builtin public entry.

## builtins/unset.c

### is_valid_identifier
Purpose:
- checks unset key syntax.

### builtin_unset
Purpose:
- removes valid keys from environment.

## env/env.c

### copy_env
Purpose:
- duplicates initial process env array for shell-owned editing.

### find_env_index
Purpose:
- finds index of key in env array.

### append_env
Purpose:
- appends new entry to env array.

### export_var
Purpose:
- parses export argument and routes to update or existing insertion.

### remove_env_index
Purpose:
- removes one environment entry and compacts array.

## env/env_utils.c

### count_env
Purpose:
- counts entries in env array.

### make_env_entry
Purpose:
- allocates key value format string.

### export_existing
Purpose:
- handles export of key without assignment when absent.

### export_update
Purpose:
- handles assignment update insertion and environ sync.

### copy_env_skip
Purpose:
- helper copy function for unset remove logic.

## redirections/redirections.c

### redirect_input_file
Purpose:
- opens file read-only and maps to stdin.

### redirect_output_truncate
Purpose:
- opens file truncating write target and maps to stdout.

### redirect_output_append
Purpose:
- opens file append write target and maps to stdout.

### apply_single_redir
Purpose:
- dispatches one redirection node by type.

### apply_redirections
Purpose:
- loops all redirection nodes and applies them in order.
Important behavior:
- first failure aborts command setup.

## redirections/redirections_util.c

### redirect_heredoc
Purpose:
- collects lines until delimiter and maps collected stream to stdin.

## 4) Key Behavioral Contracts For Evaluation

### Exit status contracts
- command not found should map to 127.
- permission denied and non-executable targets map to 126.
- syntax parser errors map to 2.
- signaled child maps to 128 plus signal number.

### Pipeline contracts
- each stage forks.
- parent tracks all pids.
- only last stage status becomes shell.last_exit.
- child signal handling is default.

### Redirection contracts
- redirections are applied per command.
- within one command redirection order matters and last redirection on same stream wins.
- in pipeline child setup, pipe wiring establishes defaults then command redirections may override those defaults.

### Builtin contracts
- state-mutating builtins must affect parent shell state in single-command path.
- exit builtin should set should_exit behavior correctly with argument validation.

## 5) Quick Practical Revision Workflow

Before evaluation, practice this speaking flow:
1. explain prompt loop in one minute.
2. explain parse path from semicolons to pipeline nodes.
3. explain one command execution path and one pipeline path.
4. explain how status code is determined in each path.
5. explain one tricky edge case you fixed, such as path resolution or redirection precedence in pipelines.

## 6) Frequently Asked Questions

### Why keep both redirection list and legacy fields in command?
To preserve compatibility with existing execution paths while supporting multiple redirections in parse order.

### Why does only the last pipeline stage define dollar question mark value?
Because standard shell behavior tracks status of the rightmost command in pipeline by default.

### Why do we reset signals in child before exec?
So external commands behave with normal terminal signal semantics.

### Why does parser store status in shell?
To let prompt and executor use unified error propagation without relying on many globals.

### Why save and restore stdio around builtin redirections?
Because builtins execute in parent for state changes, so temporary fd remapping must be reverted.

### How do you debug parser crashes quickly?
Trace token extraction and redirection parsing cursor movement first, then validate allocation and null checks.

### How do you debug pipeline output mismatches?
Check order of child dup2 wiring and apply_redirections, then verify parent closes unused ends correctly.

### What should you demonstrate confidently in evaluation?
- where parsing happens.
- where expansion happens.
- where fork and exec happen.
- where wait and status mapping happen.
- where memory is freed after each segment.

## 7) Suggested Next Study Step

If you want an even stronger eval prep package, create one short cheat sheet with:
- 15 must-know functions.
- 10 must-know edge cases.
- 10 command examples and expected exit codes.

This full reference gives you the complete map. The cheat sheet gives speed under pressure.
