# Minishell Full Deep Dive

This document is a full revision handbook for your minishell evaluation.
It is focused on the core project implementation and its exact workflow.

Scope covered:
- All core minishell source files in root, builtins, env, executor, parser, prompt, redirections
- Internal static helpers too

Scope not expanded in detail:
- External helper libraries under printf/libft (standard utility library functions)

## 1) End-to-End Workflow

From one input line to one final exit code:

1. Prompt reads line with readline.
2. Input is split by semicolons while respecting quote states.
3. Each segment is validated for edge syntax (start/end pipe, bad empties).
4. Segment is parsed:
- Syntax checked for unmatched quotes.
- Split into pipeline stages on unquoted pipes.
- Each stage parsed into one t_cmd with args and redirections.
5. Executor expands args ($VAR, $?) according to quote flags.
6. Executor dispatches:
- Single builtin in parent, or
- External command in child, or
- Pipeline execution with fork + pipe per stage.
7. Exit status is recorded in shell state.
8. Command structures are freed.
9. Loop repeats until EOF or exit builtin sets should_exit.

## 2) Runtime Data Model You Must Know

### t_shell
- env: mutable shell environment array
- last_exit: the shell $? state
- should_exit: loop break flag for exit builtin
- parser_status: parser error status carrier

### t_cmd
- args: argv-style token array for one command
- quoted: quote metadata per arg
- redirs: linked list of redirection operations in order
- legacy fields: infile/outfile/append/heredoc/heredoc_delim for compatibility
- next: pipeline chain pointer

### t_redir
- type: input/output/append/heredoc enum
- target: filename or heredoc delimiter
- next: linked list pointer

### t_pipe_exec
- cmd: current pipeline node
- pids: child pid array for wait stage
- idx: count of created children
- fd_in: previous stage read-end
- last_pid: rightmost command pid for $? capture

## 3) Main Entry and Signal Layer

### main.c

#### free_env_copy
Purpose:
- Frees duplicated environment array at shell shutdown.

Inputs/Outputs:
- Input: char **env
- Output: none

Side effects:
- Frees each string and the container array.

Edge behavior:
- Safe with NULL.

#### signal_handler
Purpose:
- Handles SIGINT and SIGQUIT in interactive parent shell.

Inputs/Outputs:
- Input: signal number
- Output: none

Side effects:
- Updates global signal state.
- On SIGINT, refreshes prompt line through readline APIs.

Edge behavior:
- SIGQUIT is ignored in prompt mode.

#### setup_signals
Purpose:
- Installs parent signal behavior for interactive loop.

Inputs/Outputs:
- Input: none
- Output: none

Side effects:
- Registers handlers.
- Disables readline default signal capture to use custom handling.

#### main
Purpose:
- Initializes shell state and starts prompt loop.

Inputs/Outputs:
- Input: argc, argv, envp
- Output: final exit code

Side effects:
- Copies env.
- Sets up signals.
- Runs prompt_loop.
- Clears history and frees env on exit.

## 4) Prompt and Segment Orchestration

### prompt.c

#### check_segment
Purpose:
- Validates one semicolon-separated segment for local pipe errors.

Checks:
- Starts with pipe
- Ends with pipe
- Empty segment position validity

#### validate_segments
Purpose:
- Iterates all segments and applies check_segment.

Contract:
- Any invalid segment sets syntax-style error code path.

#### run_segments
Purpose:
- For each valid segment:
- parse command
- execute command
- free command list

Side effects:
- Updates shell last_exit.
- Stops if shell should_exit becomes true.

#### execute_input_segments
Purpose:
- Splits one input line by semicolons.
- Validates and executes all segments.

#### prompt_loop
Purpose:
- Core REPL.

Loop details:
- Readline prompt.
- EOF exits cleanly.
- Non-empty input added to history.
- execute_input_segments called.

### prompt_utils.c

#### trim_spaces
Purpose:
- Trims leading/trailing spaces for segment checks.

Behavior:
- In-place boundary null-termination and pointer shift.

#### is_trailing_empty_segment
Purpose:
- Distinguishes legal final-empty segment vs illegal middle empties.

#### ends_with_pipe
Purpose:
- Detects trailing pipe after trim.

#### starts_with_pipe
Purpose:
- Detects leading pipe after trim.

#### handle_parse_fail
Purpose:
- Converts parser status into shell last_exit and frees segments.

### prompt_error_token.c

#### syntax_error_token
Purpose:
- Uniform error printer for unexpected token syntax errors.

Contract:
- Sets parser status to 2-like shell syntax error behavior.

## 5) Parser Pipeline

### parser/parser.c

#### parse_command
Purpose:
- Main parser entry for one segment.

Workflow:
- parser_validate_syntax
- split_pipes
- build_cmd_list

Output:
- linked list of t_cmd stages, or NULL on failure.

#### build_cmd_list
Purpose:
- Builds the pipeline linked list from split stage strings.

Side effects:
- Calls parse_single per stage.
- Cleans partial chain on failure.

#### free_cmd_chain and free_pipeline_rest
Purpose:
- Internal cleanup helpers.

### parser/parser_syntax.c

#### parser_validate_syntax
Purpose:
- Detect unmatched quotes quickly.

Contract:
- Parser error state is set consistently if quote mismatch is found.

### parser/parser_pipes.c

#### count_pipes
Purpose:
- Counts unquoted pipe separators.

Edge behavior:
- Reports failure on unmatched quote.

#### is_pipe_split
Purpose:
- Maintains quote state and decides if current char is split pipe.

#### push_segment
Purpose:
- Copies one pipe stage substring into result array.

#### split_loop
Purpose:
- Main scan loop that fills split stage list.

#### split_pipes
Purpose:
- Public splitter around helper pipeline.

Contract:
- Rejects malformed empty/space-only segments in pipeline positions.

### parser/parser_single.c

#### parse_single
Purpose:
- Parse one pipe stage into one t_cmd.

Workflow:
- parser_count_args
- allocate cmd with args/quoted arrays
- fill_args token walk

#### fill_args
Purpose:
- Token walk that decides argument vs redirection token.

#### handle_word
Purpose:
- If token is redirection operator, parse target and attach redirection.
- Else store token in args.

#### free_partial_cmd
Purpose:
- Cleanup for partially built command objects.

### parser/parser_words.c

#### parser_extract_word
Purpose:
- Extract one token from input while respecting quotes and redirection ops.

#### get_word_len
Purpose:
- Precompute allocation length for extraction.

#### fill_redirection_word
Purpose:
- Emits <, >, <<, >> operator token forms.

#### copy_quoted_content
Purpose:
- Copies quoted payload and advances parser position.

#### fill_word
Purpose:
- Final token copy engine combining raw and quoted paths.

### parser/parser_word_utils.c

#### parser_is_redir_char
Purpose:
- Quick redirection char test.

#### parser_skip_quoted
Purpose:
- Index helper to skip over full quote block.

#### parser_get_redir_len
Purpose:
- Returns one-char or two-char redirection operator length.

### parser/parser_count_args.c

#### parser_count_args
Purpose:
- Counts how many argument slots are needed for a stage.

Contract:
- Returns -1 on quote mismatch.

#### skip_word_token, skip_quoted, is_redir_char
Purpose:
- Token counting helpers used by parser_count_args.

### parser/parser_redirections.c

#### parser_set_redirection
Purpose:
- Parses operator + filename and appends a redirection node.

Workflow:
- read operator token
- skip spaces
- read target token
- map type
- append and sync legacy fields

#### parser_add_redirection
Purpose:
- Allocates and appends one t_redir node.

#### sync_legacy_redirection
Purpose:
- Keeps legacy infile/outfile flags aligned with linked-list redirs.

#### set_input_redir
Purpose:
- Handles infile vs heredoc field updates.

#### append_redir
Purpose:
- Linked-list append helper.

### parser/parser_redir_check.c

#### parser_is_redirection
Purpose:
- String-level operator matcher for parser decisions.

### parser/parser_semicolons.c

#### split_semicolons
Purpose:
- Splits full input line into independent command segments.

#### fill_segments, fill_loop, append_segment, quote_state_changed
Purpose:
- Helpers for quote-aware semicolon splitting.

### parser/parser_semicolon_count.c

#### parser_count_semicolons
Purpose:
- Counts unquoted semicolons.

#### parser_redirection_error
Purpose:
- Uniform parser-side redirection newline error output.

#### get_redir_type
Purpose:
- Maps operator text to enum type.

### parser/parser_utils.c

#### parser_set_status / parser_get_status
Purpose:
- Encapsulated parser status state in t_shell.

#### parser_is_space / parser_skip_spaces
Purpose:
- Whitespace helpers.

#### parser_free_split
Purpose:
- Frees split arrays with known element count.

### parser/parser_errors.c

#### parser_put_unmatched_quote_error
Purpose:
- Parser unmatched quote diagnostic.

#### parser_put_pipe_error
Purpose:
- Parser pipe syntax diagnostic.

### parser/parser_split_free.c

#### free_split_array
Purpose:
- Generic NULL-terminated string-array free helper.

## 6) Redirection Engine

### redirections/redirections.c

#### redirect_input_file
Purpose:
- Open path read-only and dup2 to stdin.

#### redirect_output_truncate
Purpose:
- Open/create/truncate and dup2 to stdout.

#### redirect_output_append
Purpose:
- Open/create/append and dup2 to stdout.

#### apply_single_redir
Purpose:
- Dispatch by redirection type.

#### apply_redirections
Purpose:
- Applies full list in order.

Critical contract:
- Order matters.
- Later redirection for same stream overrides earlier one.

### redirections/redirections_util.c

#### redirect_heredoc
Purpose:
- Reads lines until delimiter and feeds stdin through a pipe.

Workflow:
- create pipe
- readline loop
- write line + newline until delimiter
- dup2 read-end onto stdin

## 7) Executor Dispatcher and Status Rules

### executor/executor.c

#### run_regular_builtin
Purpose:
- Dispatches non-exit builtins and sets shell last_exit.

#### execute_builtin
Purpose:
- Runs builtin in parent with temporary stdio redirection sandbox.

Workflow:
- detect builtin
- save stdin/stdout via dup
- apply_redirections
- run builtin
- restore_stdio

Why this matters:
- cd/export/unset changes must persist in parent process.

#### execute_external_child
Purpose:
- Child branch for non-builtin single command.

Workflow:
- set child signals default
- apply redirections
- resolve path
- execve
- on failure map error + exit

#### execute_external
Purpose:
- Parent branch for single external command.

Workflow:
- fork
- child executes execute_external_child
- parent waitpid
- map status to shell last_exit

#### execute_command
Purpose:
- One entry point for execution decisions.

Decision order:
- expand args first
- if no args after expansion, treat as success/no-op
- if pipeline exists, execute_pipeline
- else builtin path
- else external path

## 8) Argument Expansion

### executor/executor_expand.c

#### executor_expand_args
Purpose:
- Rebuilds args array after variable expansion.

#### fill_expanded_args
Purpose:
- Iterates original args and stores expanded results.

Important rule:
- empty unquoted args can be dropped
- empty quoted args are kept

#### expand_argument
Purpose:
- Scans one arg and expands all dollar expressions.

#### expand_dollar
Purpose:
- Handles $? and named variable extraction.

#### append_str
Purpose:
- Safe dynamic string append helper during expansion.

## 9) Command Path and Exec Error Mapping

### executor/executor_path.c

#### join_command_path
Purpose:
- Builds directory + slash + command path candidate.

#### path_is_executable
Purpose:
- Checks X_OK on built candidate path.

#### resolve_path_from_env
Purpose:
- PATH search implementation.

Contract:
- Returns first executable full path.
- Sets ENOENT and returns NULL when not found.

### executor/executor_utils.c

#### resolve_command_path
Purpose:
- Wrapper deciding direct path vs PATH search and special bare-name handling.

#### restore_stdio
Purpose:
- Restores saved stdio after parent builtin redirection context.

### executor/executor_exec_error.c

#### is_directory_path
Purpose:
- Detects if explicit path refers to a directory.

#### executor_exit_exec_error
Purpose:
- Maps errno and command form into shell-like message + exit code.

Mapping highlights:
- ENOENT -> command not found (127)
- EACCES or directory path -> permission style (126)
- others -> perror + code 1

## 10) Pipeline Execution

### executor/executor_pipe_init.c

#### count_pipeline_cmds
Purpose:
- Counts number of stages for pid array sizing.

#### init_pipeline_exec
Purpose:
- Initializes t_pipe_exec and allocates pid array.

### executor/executor_pipe.c

#### close_pipe_pair
Purpose:
- Safe close helper when current stage has next pipe.

#### run_pipeline_step
Purpose:
- Executes one stage lifecycle:
- expansion
- optional pipe
- fork
- child execution handoff
- parent fd tracking

#### execute_pipeline
Purpose:
- Full pipeline loop and final wait stage.

Status contract:
- final $? comes from last command pid only.

### executor/executor_pipe_utils.c

#### close_pipeline_fds
Purpose:
- Child-side fd cleanup after dup2.

#### setup_pipeline_child
Purpose:
- Wires stdin from previous stage and stdout to next stage pipe.

#### run_pipeline_execve
Purpose:
- Path resolve + execve path for pipeline child.

#### execute_pipeline_child
Purpose:
- Child execution sequence in pipeline.

Order importance:
- pipe dup setup
- apply command redirections
- execve

This order preserves correct precedence where explicit command redirections override default pipe stream mapping.

#### wait_pipeline
Purpose:
- waitpid over all stages and capture last stage status.

## 11) Builtins Deep Details

### builtins/echo_utils.c

#### is_var_start / is_var_char
Purpose:
- Variable lexer rules shared by echo expansion logic.

### builtins/echo.c

#### expand_variable
Purpose:
- Reads variable name from arg string and prints resolved value.

#### print_expansion
Purpose:
- Handles $? and named variable branch while printing echo arg.

#### print_echo_arg
Purpose:
- Prints arg literal or expansion-aware based on quote metadata.

#### builtin_echo
Purpose:
- Implements echo and -n behavior.

### builtins/cd_utils.c

#### cd_get_target
Purpose:
- Resolves target path with HOME fallback and basic argument validation.

#### cd_update_pwd
Purpose:
- Refreshes PWD and OLDPWD after successful chdir.

### builtins/cd_pwd.c

#### set_shell_env_var
Purpose:
- Utility to export a key/value pair to shell env.

#### builtin_pwd
Purpose:
- Prints current working directory.

#### builtin_cd
Purpose:
- Orchestrates directory change and env update.

### builtins/export.c

#### is_valid_identifier
Purpose:
- Identifier grammar validator for export.

#### print_export_error
Purpose:
- Invalid identifier diagnostic.

#### print_export
Purpose:
- Dumps env in declare -x style.

#### builtin_export
Purpose:
- No-arg print path or multi-arg export update path.

### builtins/unset.c

#### is_valid_identifier
Purpose:
- Identifier grammar validator for unset.

#### builtin_unset
Purpose:
- Removes variables by key if valid; sets failure if any invalid key appears.

### executor/executor_exit.c

#### get_exit_sign
Purpose:
- Parses optional sign and numeric boundary data.

#### parse_exit_code
Purpose:
- Strict conversion for exit argument with overflow handling.

#### builtin_exit
Purpose:
- Handles no arg, one arg, too-many-args, invalid numeric cases.

## 12) Environment Management

### env/env_utils.c

#### count_env
Purpose:
- Counts entries in env array.

#### make_env_entry
Purpose:
- Creates KEY=VALUE string.

#### export_existing
Purpose:
- Adds bare key if missing.

#### export_update
Purpose:
- Upserts KEY=VALUE and syncs global environ.

#### copy_env_skip
Purpose:
- Rebuild helper when deleting one entry.

### env/env.c

#### copy_env
Purpose:
- Duplicates startup environment.

#### find_env_index
Purpose:
- Finds variable index by key in env array.

#### append_env
Purpose:
- Realloc append for env array.

#### export_var
Purpose:
- Splits argument into key/value semantics and delegates update path.

#### remove_env_index
Purpose:
- Removes one env element and compacts array.

## 13) Memory Ownership and Cleanup Rules

What each major layer owns:
- Parser allocates t_cmd, args, quoted, redirs, legacy redir strings.
- Executor may replace args/quoted after expansion and must free originals.
- prompt layer always frees cmd list after each segment execution.
- env mutation functions replace shell env arrays carefully and sync global environ.

Leak-prevention points:
- parse failure must free partial command allocations.
- pipeline init failure must not leak pid array.
- exec failure path in child must free path string before error exit.
- builtin redirection wrapper must restore stdio even on redirection failure.

## 14) Exit Code and Error Contracts

You should memorize these:
- 0: success
- 1: generic runtime/builtin error
- 2: syntax-level issues and invalid numeric exit argument cases
- 126: found but not executable or directory execution context
- 127: command not found path
- 128 + signal: child terminated by signal

Pipeline contract:
- final shell $? is the rightmost command status.

## 15) Redirection and Pipe Precedence

For each command stage:
- Pipe wiring gives default stdin/stdout for stage position.
- Redirections are then applied for that command.
- Result: explicit command redirections override default pipe streams.

Typical intuition examples:
- echo hi >file | echo bye
- first stage writes to file, not into pipe
- second stage still runs and prints bye

- echo hi | cat <infile
- right stage reads infile, not left pipe data

## 16) Signals Contract

Parent interactive shell:
- SIGINT refreshes prompt line cleanly.
- SIGQUIT ignored.

Children before execve:
- SIGINT and SIGQUIT reset to default.

This separation is required so the shell UI stays stable while external commands behave normally.

## 17) Practical Evaluation Storyboard

When evaluator asks Explain your shell workflow, say:

1. Prompt loop reads raw input.
2. Semicolon splitter produces independent segments.
3. Each segment is syntax-validated and pipe-split.
4. Each pipe stage becomes one t_cmd with args + redir list.
5. Executor expands variables.
6. Dispatcher chooses builtin, external, or pipeline.
7. Redirections are applied in command order.
8. Pipeline waits all children and keeps last stage exit code.
9. Command structures are fully freed.
10. Loop continues until exit/EOF.

## 18) Fast Self-Check Before Evaluation

Use this checklist:
- Can I explain why builtins like cd/export/unset run in parent?
- Can I explain why pipeline final status comes from last pid?
- Can I explain 126 vs 127 with explicit examples?
- Can I explain quote effects on expansion?
- Can I explain redirection precedence over pipe defaults?
- Can I trace memory ownership for t_cmd and t_redir?
- Can I describe signal differences parent vs child?

If you can answer all 7 without looking at code, you are well prepared.

## 19) Mini FAQ

Q: Why does export in a pipeline not persist?
A: Pipeline stage runs in child process context, so env mutations do not update parent shell state.

Q: Why do we keep legacy infile/outfile fields if we already have redir list?
A: Compatibility and convenience with existing execution paths while parser maintains full ordered redirection list.

Q: Why is unmatched quote treated as syntax-level failure?
A: Parser cannot produce reliable token boundaries when quote state is open, so command is rejected early.

Q: Why can command output file differ in pipeline edge cases?
A: If pipe/dup2 and redirection application order is wrong, stream precedence flips. Correct order keeps redirection override behavior.

Q: Why do we duplicate stdin/stdout around builtins?
A: Builtins run in parent, so redirections must be temporary and restored to avoid corrupting next prompt iteration streams.

Q: Why do we pass shell->env to execve instead of environ directly?
A: shell->env is your controlled mutable state and is synchronized intentionally after export/unset operations.
