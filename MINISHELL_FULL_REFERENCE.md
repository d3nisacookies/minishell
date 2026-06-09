# Minishell Full Reference and Current Code Walkthrough

This version matches the current source tree in this repository. It covers the core shell only and excludes `printf/` and `printf/libft/`.

The notes below explain functions **line by line in grouped source ranges** so you can read the code quickly during revision.

## 1) End-to-End Runtime Flow

1. `main` initializes `t_shell`, duplicates the environment, bumps `SHLVL`, installs signals, and enters `prompt_loop`.
2. `prompt_loop` reads one line with `readline`.
3. `split_semicolons` cuts the raw line into top-level segments while respecting quotes.
4. `check_segment` rejects bad `;` and `|` placements before deeper parsing.
5. `parse_command` validates quotes, splits by unquoted pipes, and turns each stage into a `t_cmd`.
6. `executor_expand_args` expands `$VAR` and `$?`.
7. The executor picks one of three paths: single builtin, single external command, or pipeline.
8. Redirections are applied in list order, so later redirections override earlier ones on the same stream.
9. Exit status is written back to `shell->last_exit`.
10. Every parsed command chain is freed before the next prompt iteration.

## 2) Core Data Structures

### `t_shell`
- `env`: live environment passed to `execve`.
- `exported`: export table used by `export` even for keys without `=`.
- `last_exit`: current `$?`.
- `should_exit`: loop stop flag used by `exit`.
- `parser_status`: parser-side error storage.

### `t_cmd`
- `args` / `quoted`: argv array plus quote metadata per token.
- `argc`: current argument count.
- `infile`, `outfile`, `append`, `heredoc`, `heredoc_delim`: compatibility fields kept in sync with the redirection list.
- `redirs`: ordered linked list of redirections.
- `next`: next pipeline stage.

### `t_redir`
- `type`: `R_IN`, `R_OUT`, `R_APPEND`, `R_HEREDOC`.
- `target`: file name or heredoc delimiter.
- `quoted`: whether the original target came from a quoted token.
- `next`: next redirection in command order.

### `t_pipe_exec`
- `cmd`: current stage being processed.
- `pids`: all child pids in the pipeline.
- `idx`: next slot in `pids`.
- `fd_in`: read end from the previous stage.
- `last_pid`: pid of the rightmost stage, used for final `$?`.

## 3) Startup and Prompt Layer

### `main.c`

#### `signal_handler` (`main.c:20-30`)
- `22` stores the received signal in `g_signal`.
- `23-29` only special-case `SIGINT`: print a newline, tell readline a new line started, clear the pending buffer, and redraw the prompt.

#### `setup_signals` (`main.c:32-46`)
- `34-42` prepares one `sigaction` for `SIGINT` and one ignored action for `SIGQUIT`.
- `43` disables readline's built-in signal handling so minishell owns prompt redraw behavior.
- `44-45` installs both handlers.

#### `main` (`main.c:48-65`)
- `53-54` ignores unused `ac` and `av`.
- `55-56` initializes shell-owned environment state and aborts early on failure.
- `57-60` seeds shell status fields, increments `SHLVL`, and installs signals.
- `61` starts the REPL.
- `62-64` captures final status, clears readline history, and frees shell state.
- `65` returns the last exit code.

### `prompt.c`

#### `check_segment` (`prompt.c:15-41`)
- `20` trims surrounding blanks before validating syntax.
- `21-27` handles normal non-empty segments:
  - `23-24` rejects a leading `|`.
  - `25-26` rejects a segment that ends with `|` when another `;` segment follows.
- `29-33` checks whether an empty segment was created right after a piped segment and turns that into a `;` syntax error.
- `35-39` distinguishes legal trailing empties from illegal `;;` or bare `;` cases.
- `41` returns `1` only for the special "legal trailing empty segment" case.

#### `validate_segments` (`prompt.c:44-59`)
- `49-57` walks every semicolon segment and delegates validation to `check_segment`.
- `52-56` stops immediately on hard failure or on the legal trailing-empty sentinel.
- `59` returns success when all checked segments are valid.

#### `run_segments` (`prompt.c:62-83`)
- `68-80` loops over validated segments in order.
- `71-73` trims and skips the trailing empty segment case.
- `74-76` parses one segment; parser failure is routed through `handle_parse_fail`.
- `77-78` executes the command chain and frees it right after execution.
- `79-80` stops early if `exit` set `should_exit`.

#### `execute_input_segments` (`prompt.c:86-108`)
- `91` splits the raw line on semicolons while preserving quoted content.
- `92-98` converts split failure into a shell exit code using `parser_status`.
- `100-104` validates top-level segment syntax and frees the split array on failure.
- `106-107` runs all segments and then frees the segment array.

#### `prompt_loop` (`prompt.c:111-135`)
- `117` reads the next prompt line.
- `118-121` translates a prompt-time `Ctrl-C` into shell status `130` and resets `g_signal`.
- `122-123` treats EOF as clean shell termination after printing a newline.
- `124-127` ignores empty lines.
- `129-130` stores non-empty lines in history and executes them.
- `131-133` frees the input and exits the loop if `exit` requested shutdown.

### `prompt_utils.c`

#### `trim_spaces` (`prompt_utils.c:15-29`)
- `20-21` guard against `NULL`.
- `22-24` moves `start` past leading spaces and tabs.
- `25-27` moves `end` backward past trailing spaces and tabs.
- `28-29` writes a new `'\0'` at the trimmed end and returns the first non-space character.

#### `is_trailing_empty_segment` (`prompt_utils.c:32-38`)
- Returns true only for an empty segment that appears at the very end and is not the first segment.

#### `ends_with_pipe` (`prompt_utils.c:41-50`)
- Trims trailing blanks conceptually and tests whether the final visible character is `|`.

#### `starts_with_pipe` (`prompt_utils.c:53-60`)
- Skips leading blanks and tests whether the first visible character is `|`.

#### `handle_parse_fail` (`prompt_utils.c:63-72`)
- Reads `parser_status`, falls back to generic status `1` if needed, copies it to `last_exit`, frees the segment array, and returns `-1`.

### `prompt_error_token.c`

#### `syntax_error_token` (`prompt_error_token.c:15-22`)
- `17` stores parser status `2`.
- `18-20` prints the standard unexpected-token message.
- `21` mirrors the same code into `shell->last_exit`.
- `22` returns failure.

## 4) Parser

### `parser/parser.c`

#### `free_cmd_chain` (`parser.c:15-25`)
- Walks a partially built pipeline list and frees each node with `free_cmd`.

#### `free_pipeline_rest` (`parser.c:27-38`)
- Frees any unconsumed strings left in the split pipeline array, then frees the array itself.

#### `append_cmd` (`parser.c:40-53`)
- Parses one pipeline segment with `parse_single`.
- Attaches the new node either as the head or after the current tail.

#### `build_cmd_list` (`parser.c:56-75`)
- `63-73` parses every pipeline string.
- `65-70` cleans both the remaining split strings and the already-built chain on failure.
- `72-74` frees each consumed segment and finally the container array.

#### `parse_command` (`parser.c:78-92`)
- `83` clears the previous parser status.
- `84-85` rejects unmatched-quote input early.
- `86-88` splits the segment on unquoted pipes.
- `89-91` builds the linked list of `t_cmd` stages.
- `92` returns the pipeline head.

### `parser/parser_count_args.c`

#### `is_redir_char` (`parser_count_args.c:15-17`)
- Treats only `<` and `>` as redirection token starters.

#### `skip_quoted` (`parser_count_args.c:20-29`)
- Remembers the opening quote, advances until the matching closing quote, and returns `-1` if none is found.

#### `skip_word_token` (`parser_count_args.c:32-43`)
- Advances through one non-redirection token.
- Delegates quoted sections to `skip_quoted`.

#### `parser_count_args` (`parser_count_args.c:46-66`)
- `53-64` walks the input token by token.
- Counts both normal words and redirection operators so `alloc_cmd` knows the maximum array size needed.
- Returns `-1` on quote mismatch.

### `parser/parser_errors.c`

#### `parser_put_quote_error` (`parser_errors.c:15-19`)
- Sets parser status `2` and prints the unmatched-quote diagnostic.

#### `parser_put_pipe_error` (`parser_errors.c:21-24`)
- Sets parser status `2` and prints the unexpected-pipe diagnostic.

### `parser/parser_pipes.c`

#### `count_pipes` (`parser_pipes.c:15-36`)
- Tracks quote state while scanning.
- Counts only unquoted `|`.
- Returns `-1` if the scan ends with an open quote.

#### `push_segment` (`parser_pipes.c:39-58`)
- Trims only leading blanks inside the `[start, end)` range.
- Rejects empty segments.
- Copies the stage text into the result array.

#### `is_pipe_split` (`parser_pipes.c:61-75`)
- Updates quote state on quote characters.
- Returns true only when an unquoted `|` is found.

#### `split_loop` (`parser_pipes.c:78-104`)
- Scans once through the input.
- On each unquoted `|`, saves the preceding range through `push_segment`.
- Pushes the final trailing range after the loop.

#### `split_pipes` (`parser_pipes.c:107-123`)
- `113-115` counts stages and reports unmatched quotes.
- `116-118` allocates the result array.
- `120-121` runs the split and converts empty-stage failures into a pipe syntax error.
- `122` null-terminates the array.

### `parser/parser_redir_check.c`

#### `parser_is_redirection` (`parser_redir_check.c:15-27`)
- Matches exactly `>`, `>>`, `<`, or `<<`.

### `parser/parser_redirections.c`

#### `append_redir` (`parser_redirections.c:15-28`)
- Appends a new redirection node at the end of `cmd->redirs` to preserve source order.

#### `set_input_redir` (`parser_redirections.c:30-47`)
- For heredoc: sets `heredoc`, replaces `heredoc_delim`, and clears `infile`.
- For normal input redirection: clears heredoc state and stores the new `infile`.

#### `sync_legacy_redirection` (`parser_redirections.c:50-65`)
- Duplicates the target string.
- Updates the compatibility fields (`infile`, `outfile`, `append`, `heredoc`, `heredoc_delim`) so old execution paths still reflect the latest redirection.

#### `parser_add_redirection` (`parser_redirections.c:68-83`)
- Allocates `t_redir`, fills its fields, syncs the legacy fields, and appends the node to the linked list.

#### `parser_set_redirection` (`parser_redirections.c:86-105`)
- `94-96` extracts the operator token.
- `97-99` skips spaces and throws a newline syntax error if no target follows.
- `100-102` extracts the redirection target token and remembers whether it was quoted.
- `103-105` maps the operator to `t_redir_type`, frees the temporary operator string, and stores the redirection.

### `parser/parser_semicolon_count.c`

#### `parser_count_semicolons` (`parser_semicolon_count.c:15-34`)
- Tracks quote state and counts only unquoted `;`.

#### `parser_redirection_error` (`parser_semicolon_count.c:37-43`)
- Stores parser status `2`, prints the unexpected-newline message, frees the operator string, and returns failure.

#### `get_redir_type` (`parser_semicolon_count.c:46-54`)
- Maps `<`, `<<`, `>>`, and default `>` to the enum used by the redirection list.

### `parser/parser_semicolons.c`

#### `quote_state_changed` (`parser_semicolons.c:15-21`)
- Toggles quote state when entering or leaving quoted text.

#### `append_segment` (`parser_semicolons.c:24-29`)
- Writes one already-allocated segment into the split list and returns the next insertion index.

#### `fill_loop` (`parser_semicolons.c:32-52`)
- Scans the raw input once.
- When it finds an unquoted `;`, it copies the segment from `start` up to the delimiter and moves `start` past the semicolon.

#### `fill_segments` (`parser_semicolons.c:55-76`)
- Initializes quote state and delegates the main scan to `fill_loop`.
- Rejects unmatched quotes after the scan.
- Copies the last segment after the final semicolon and null-terminates the list.

#### `split_semicolons` (`parser_semicolons.c:79-93`)
- Counts how many semicolons exist to size the output array.
- Allocates the list and fills it.
- Frees partial state on failure.

### `parser/parser_single.c`

#### `free_partial_cmd` (`parser_single.c:15-21`)
- Null-terminates the partially built `args` array and then frees the command safely.

#### `handle_word` (`parser_single.c:24-41`)
- Dispatches `<` and `>` tokens to `parser_set_redirection`.
- Otherwise extracts a word token.
- Stores normal words in `args` and `quoted`.
- Rejects bare redirection tokens that slipped through as regular words.

#### `fill_args` (`parser_single.c:44-59`)
- Walks the command text token by token.
- Skips blanks, sends each token through `handle_word`, then null-terminates `args`.

#### `alloc_cmd` (`parser_single.c:62-83`)
- Allocates the command itself, then `args`, then `quoted`.
- Initializes all legacy redirection fields and bookkeeping members to safe defaults.

#### `parse_single` (`parser_single.c:86-108`)
- Rejects empty input.
- Counts tokens up front.
- Allocates the command.
- Fills arguments and redirections.
- On failure, frees the partial command and reports unmatched quotes.

### `parser/parser_split_free.c`

#### `free_split_array` (`parser_split_free.c:15-27`)
- Frees a null-terminated string array and its container.

### `parser/parser_syntax.c`

#### `parser_validate_syntax` (`parser_syntax.c:15-32`)
- Runs a quote-balance scan only.
- Returns failure if the scan ends while still inside a quote.

### `parser/parser_utils.c`

#### `parser_set_status` (`parser_utils.c:15-17`)
- Writes the parser status into `t_shell`.

#### `parser_get_status` (`parser_utils.c:20-22`)
- Reads the parser status back out of `t_shell`.

#### `parser_is_space` (`parser_utils.c:25-27`)
- Treats only space and tab as parser whitespace.

#### `parser_skip_spaces` (`parser_utils.c:30-33`)
- Advances an index over consecutive parser whitespace.

#### `parser_free_split` (`parser_utils.c:36-46`)
- Frees only the first `count` entries of a split array plus the container.

### `parser/parser_word_utils.c`

#### `parser_is_redir_char` (`parser_word_utils.c:15-17`)
- Single-character predicate for `<` and `>`.

#### `parser_skip_quoted` (`parser_word_utils.c:20-29`)
- Skips from the opening quote to the matching closing quote and reports failure if it never closes.

#### `parser_get_redir_len` (`parser_word_utils.c:32-38`)
- Returns `2` for `<<` or `>>`, `1` for `<` or `>`, and `0` otherwise.

### `parser/parser_words.c`

#### `get_word_len` (`parser_words.c:15-36`)
- Returns redirection operator length directly for `<`, `>`, `<<`, `>>`.
- Otherwise counts how many output characters the final token needs.
- Quoted delimiters are excluded from the final length.

#### `fill_redirection_word` (`parser_words.c:39-48`)
- Copies one- or two-character redirection operators into the token buffer.

#### `copy_quoted_content` (`parser_words.c:52-62`)
- Skips the opening quote, copies only the inner content, stops at the matching quote, and returns that quote character.

#### `fill_word` (`parser_words.c:65-84`)
- Handles either an operator token or a normal word.
- Preserves raw characters.
- Removes quote delimiters while storing the quote type in `was_quoted`.

#### `parser_extract_word` (`parser_words.c:87-104`)
- Computes required length, allocates the token buffer, resets `was_quoted`, and fills the token.

## 5) Redirections

### `redirections/redirections.c`

#### `ambiguous_redirect` (`redirections.c:15-22`)
- Prints `<original target>: ambiguous redirect`, frees the expanded candidate, sets exit status `1`, and returns failure.

#### `expand_redir_target` (`redirections.c:25-36`)
- Leaves heredoc delimiters untouched.
- Expands all other redirection targets with `expand_argument`.
- Rejects unquoted expansions that become empty or contain spaces/tabs.

#### `apply_single_redir` (`redirections.c:39-47`)
- Dispatches each redirection type to the matching low-level helper.

#### `apply_redirections` (`redirections.c:51-73`)
- Walks the redirection list in source order.
- Expands targets first, then applies the low-level redirection.
- Frees temporary expanded targets only when expansion allocated a new string.

### `redirections/redirections_io.c`

#### `redirect_input_file` (`redirections_io.c:15-25`)
- Opens the file read-only, `dup2`s it to `STDIN_FILENO`, closes the original fd, and reports failures with `perror`.

#### `redirect_output_truncate` (`redirections_io.c:28-38`)
- Opens or creates the file with truncation, `dup2`s it onto stdout, then closes the original fd.

#### `redirect_output_append` (`redirections_io.c:41-51`)
- Opens or creates the file in append mode, `dup2`s it onto stdout, then closes the original fd.

### `redirections/redirections_util.c`

#### `heredoc_sigint` (`redirections_util.c:15-22`)
- Marks `SIGINT`, prints a newline, clears the readline buffer, and forces readline to stop.

#### `setup_heredoc_signals` (`redirections_util.c:24-44`)
- Installs a special `SIGINT` handler for heredoc reading and ignores `SIGQUIT`.
- Restores the previous `SIGINT` handler if the `SIGQUIT` install fails.

#### `write_heredoc_line` (`redirections_util.c:47-60`)
- Duplicates the line literally when the delimiter was quoted.
- Otherwise expands variables before writing the line plus a newline into the pipe.

#### `fill_heredoc_pipe` (`redirections_util.c:63-81`)
- Reads `> ` lines until EOF, delimiter match, or `Ctrl-C`.
- Writes each accepted line through `write_heredoc_line`.

#### `redirect_heredoc` (`redirections_util.c:84-110`)
- Creates a pipe.
- Installs heredoc-specific signals.
- Fills the write end with user input.
- Restores normal signals.
- `dup2`s the read end onto stdin so the command reads heredoc content from standard input.

## 6) Executor

### `executor/executor.c`

#### `run_regular_builtin` (`executor.c:15-30`)
- Dispatches `echo`, `cd`, `pwd`, `export`, and `unset`.
- Stores builtin status back into `shell->last_exit`.

#### `execute_builtin` (`executor.c:32-53`)
- `37-38` handles `exit` separately because it only flips shell state.
- `39-43` rejects non-builtins quickly.
- `44-48` snapshots stdin/stdout with `dup`.
- `49-50` applies command redirections in the parent.
- `51-52` runs the builtin and restores stdio.

#### `execute_external_child` (`executor.c:56-75`)
- Restores default signal behavior for child processes.
- Applies redirections.
- Resolves the executable path.
- Calls `execve`.
- Falls back to `executor_exit_exec_error` if execution fails.

#### `execute_external` (`executor.c:77-92`)
- Forks once.
- Runs the child path in the forked process.
- Waits in the parent and maps either normal exit or signal death into `shell->last_exit`.

#### `execute_command` (`executor.c:94-106`)
- Rejects empty command objects.
- Expands arguments first.
- Treats "all args disappeared after expansion" as a no-op success.
- Chooses pipeline, builtin, or external execution in that order.

### `executor/executor_exec_error.c`

#### `is_directory_path` (`executor_exec_error.c:16-26`)
- Only checks explicit paths containing `/`.
- Uses `stat` to distinguish directories from missing paths.

#### `executor_exit_exec_error` (`executor_exec_error.c:29-50`)
- Maps directory execution to exit `126`.
- Maps `ENOENT` to "command not found" and exit `127`.
- Maps `EACCES` to "Permission denied" and exit `126`.
- Uses `perror` and exit `1` for everything else.

### `executor/executor_exit.c`

#### `get_exit_sign` (`executor_exit.c:15-29`)
- Initializes parsing state for `exit`.
- Handles optional `+` or `-`.
- Adjusts the allowed numeric limit for negative overflow handling.

#### `parse_exit_code` (`executor_exit.c:32-55`)
- Rejects empty or non-numeric content.
- Detects unsigned overflow before multiplying by `10`.
- Produces the final 8-bit shell exit code.

#### `builtin_exit` (`executor_exit.c:58-83`)
- No argument: set `should_exit`.
- Invalid numeric argument: print error, set status `2`, and exit.
- Too many arguments: print error, keep shell alive, set status `1`.
- Single valid argument: store the code and exit.

### `executor/executor_expand.c`

#### `fill_expanded_args` (`executor_expand.c:15-34`)
- Expands each original argument.
- Frees the old string immediately after expansion.
- Keeps quoted empty strings.
- Drops unquoted empty expansion results.

#### `executor_expand_args` (`executor_expand.c:37-55`)
- Allocates replacement `args` and `quoted` arrays.
- Fills them through `fill_expanded_args`.
- Swaps them into the command and updates `argc`.

### `executor/executor_expand_arg.c`

#### `append_str` (`executor_expand_arg.c:15-25`)
- Returns the original destination if `src` is `NULL`.
- Duplicates `src` when starting from an empty destination.
- Otherwise joins and frees the old destination.

#### `expand_variable` (`executor_expand_arg.c:28-41`)
- Moves past `$`, scans a valid variable name, extracts the key, fetches the env value, and appends it to the output buffer.

#### `expand_dollar` (`executor_expand_arg.c:44-62`)
- `48-54` expands `$?`.
- `56-60` keeps a literal `$` when no valid variable name follows.
- `62` delegates normal `$NAME` expansion to `expand_variable`.

#### `expand_argument` (`executor_expand_arg.c:65-86`)
- Returns the argument unchanged when the token came from single quotes.
- Builds a new string left to right.
- Copies plain characters literally.
- Routes every `$` through `expand_dollar`.

### `executor/executor_free.c`

#### `free_redirections` (`executor_free.c:15-24`)
- Frees the full redirection linked list and each `target`.

#### `free_cmd` (`executor_free.c:28-43`)
- Frees argument strings, redirection list, both arrays, legacy redirection fields, and finally the command itself.

#### `free_cmd_list` (`executor_free.c:46-55`)
- Iterates a full pipeline and frees one node at a time.

### `executor/executor_path.c`

#### `join_command_path` (`executor_path.c:15-26`)
- Handles empty path elements by treating them as the current directory.
- Otherwise builds `dir + "/" + cmd`.

#### `path_is_executable` (`executor_path.c:29-38`)
- Builds one candidate path and keeps it only if `access(..., X_OK)` succeeds.

#### `resolve_path_from_env` (`executor_path.c:41-63`)
- Reads `PATH`.
- Splits it on `:`.
- Tests each directory with `path_is_executable`.
- Returns the first executable full path or `NULL` with `errno = ENOENT`.

### `executor/executor_pipe.c`

#### `close_pipe_pair` (`executor_pipe.c:15-21`)
- Closes both ends of the just-created pipe when a next stage exists.

#### `run_pipeline_step` (`executor_pipe.c:24-48`)
- Expands the current stage args.
- Creates a pipe if another stage follows.
- Forks one child for the stage.
- Stores the child pid, tracks `last_pid`, closes the previous `fd_in`, and keeps the next read end for the following stage.

#### `execute_pipeline` (`executor_pipe.c:51-65`)
- Initializes `t_pipe_exec`.
- Runs every stage until one step fails.
- Closes any leftover read end.
- Waits all children and frees the pid array.

### `executor/executor_pipe_init.c`

#### `count_pipeline_cmds` (`executor_pipe_init.c:15-24`)
- Counts how many `t_cmd` nodes exist in the chain.

#### `init_pipeline_exec` (`executor_pipe_init.c:28-40`)
- Allocates the pid array and initializes `idx`, `fd_in`, `last_pid`, and `cmd`.

### `executor/executor_pipe_utils.c`

#### `close_pipeline_fds` (`executor_pipe_utils.c:15-23`)
- Closes inherited `fd_in` and the current pipe pair in the child once `dup2` wiring is done.

#### `run_pipeline_execve` (`executor_pipe_utils.c:26-51`)
- Returns immediately for an empty stage.
- Runs `exit` and the regular builtins inside the child process when they appear in a pipeline.
- Otherwise resolves the executable path and calls `execve`.

#### `setup_pipeline_child` (`executor_pipe_utils.c:54-60`)
- Connects previous stage input to stdin.
- Connects the current pipe write end to stdout when another stage follows.
- Closes redundant fds afterward.

#### `wait_pipeline` (`executor_pipe_utils.c:63-79`)
- Waits every pid.
- Updates `shell->last_exit` only for `last_pid`, which matches normal shell pipeline semantics.

#### `execute_pipeline_child` (`executor_pipe_utils.c:82-95`)
- Restores default signals.
- Wires pipe fds.
- Applies per-command redirections after pipe defaults, so explicit redirections win.
- Dispatches execution through `run_pipeline_execve`.

### `executor/executor_utils.c`

#### `resolve_command_path` (`executor_utils.c:27-38`)
- Rejects empty command names.
- Returns explicit paths unchanged when they already contain `/`.
- Otherwise searches `PATH`.

#### `handle_builtin_redir_error` (`executor_utils.c:41-46`)
- Restores stdio snapshots and leaves `last_exit` at `1` unless the failure came from heredoc `SIGINT`.

#### `restore_stdio` (`executor_utils.c:49-60`)
- `dup2`s saved stdin/stdout back into place and closes the saved descriptors.

## 7) Builtins

### `builtins/cd_path.c`

#### `join_cd_path` (`cd_path.c:15-31`)
- Returns the target unchanged for absolute paths.
- Falls back to the target unchanged when there is no usable base path.
- Otherwise builds `base/target`.

#### `filter_cd_parts` (`cd_path.c:34-56`)
- Removes empty parts and `.`.
- Collapses `..` by deleting the previous kept part when possible.
- Compacts the array in place and returns the final depth.

#### `cd_path_len` (`cd_path.c:59-68`)
- Computes the output buffer length for the normalized absolute path.

#### `build_cd_path` (`cd_path.c:71-94`)
- Returns `/` when every part cancels out.
- Allocates the normalized path.
- Copies each remaining part separated by single `/`.

#### `resolve_cd_pwd` (`cd_path.c:97-114`)
- Joins old `PWD` with the user target.
- Splits the combined path on `/`.
- Normalizes `.` and `..`.
- Rebuilds the final logical `PWD`.

### `builtins/cd_pwd.c`

#### `set_shell_env_var` (`cd_pwd.c:15-31`)
- Builds `KEY=VALUE`, passes it through `export_var`, and frees the temporary string.

#### `get_env_value` (`cd_pwd.c:34-51`)
- Looks in `shell->env` first.
- Returns `""` for exported keys without a value.
- Returns `NULL` when the key is absent from both tables.

#### `builtin_pwd` (`cd_pwd.c:54-72`)
- Prefers the logical `PWD` value from the environment.
- Falls back to `getcwd` if `PWD` is unset or empty.

#### `builtin_cd` (`cd_pwd.c:75-95`)
- Validates arguments and resolves the target.
- Saves old `PWD`.
- Calls `chdir`.
- Updates `PWD` and `OLDPWD` through `cd_update_pwd`.

### `builtins/cd_utils.c`

#### `cd_get_target` (`cd_utils.c:15-34`)
- Rejects more than one argument.
- Uses `cmd->args[1]` when present.
- Expands a simple `$NAME` target.
- Falls back to `HOME`.

#### `cd_update_pwd` (`cd_utils.c:37-52`)
- Computes the new logical path with `resolve_cd_pwd`.
- Updates `OLDPWD` if the old path was known.
- Updates `PWD` if normalization succeeded.

### `builtins/echo.c`

#### `is_n_option` (`echo.c:15-24`)
- Accepts `-n`, `-nn`, `-nnn`, and similar forms.

#### `print_echo_args` (`echo.c:27-38`)
- Prints remaining arguments with single spaces between them.

#### `builtin_echo` (`echo.c:41-54`)
- Skips all leading valid `-n` options.
- Prints the remaining args.
- Prints a newline only when no `-n` option was consumed.

### `builtins/echo_utils.c`

#### `is_var_start` (`echo_utils.c:15-17`)
- Valid first character for variable names: alphabetic or `_`.

#### `is_var_char` (`echo_utils.c:20-22`)
- Valid continuation character: alphanumeric or `_`.

### `builtins/export.c`

#### `is_valid_identifier` (`export.c:15-30`)
- Requires the first character to be alphabetic or `_`.
- Allows alphanumeric and `_` until `=` or string end.

#### `print_export_error` (`export.c:33-37`)
- Prints the standard invalid-identifier message for `export`.

#### `print_export_value` (`export.c:40-46`)
- Prints `declare -x KEY="VALUE"` for entries that contain `=`.

#### `print_export` (`export.c:49-65`)
- Prints every exported entry.
- Uses a plain `declare -x KEY` form when the entry has no value.

#### `builtin_export` (`export.c:68-91`)
- Initializes builtin status to success.
- With no extra args, prints the export table.
- Otherwise validates each argument and either exports it or reports an invalid identifier.

### `builtins/unset.c`

#### `is_valid_identifier` (`unset.c:15-30`)
- Same identifier rule as `export`, but the whole string must be a valid name because `unset` does not accept `=`.

#### `unset_key` (`unset.c:33-42`)
- Removes the key from both the live env table and the export table when present.

#### `builtin_unset` (`unset.c:45-65`)
- Starts with success status.
- Validates each key.
- Reports invalid identifiers but continues processing the remaining keys.

## 8) Environment Management

### `env/env.c`

#### `copy_env` (`env.c:16-35`)
- Allocates a new `char **`.
- Duplicates every entry from `envp`.
- Null-terminates the new array.

#### `find_env_index` (`env.c:38-54`)
- Matches a key only when the next character is `=` or string end, so `PATH` does not match `PATHNAME`.

#### `append_env` (`env.c:57-79`)
- Reallocates the environment pointer array one slot larger.
- Reuses existing entry pointers.
- Appends the already-allocated `new_entry`.

#### `export_var` (`env.c:81-98`)
- Splits `KEY=VALUE` form into key and value.
- Delegates bare keys to `export_existing`.
- Delegates assignments to `export_update`.

### `env/env_init.c`

#### `free_env_copy` (`env_init.c:17-29`)
- Frees a duplicated `char **` array entry by entry.

#### `is_numeric_value` (`env_init.c:32-49`)
- Accepts optional leading sign.
- Requires at least one digit.
- Rejects any non-digit content.

#### `init_shell_state` (`env_init.c:52-64`)
- Duplicates the startup environment twice: once for `env`, once for `exported`.
- Cleans up properly if the second copy fails.
- Points the global `environ` at `shell->env`.

#### `init_shlvl` (`env_init.c:67-82`)
- Reads current `SHLVL`.
- If it is numeric, increments it; otherwise resets it to `1`.
- Writes the new value back through the normal env update path.

#### `free_shell_state` (`env_init.c:85-90`)
- Frees both `env` and `exported`.

### `env/env_remove.c`

#### `copy_env_skip` (`env_remove.c:17-32`)
- Copies every pointer except the one being removed.
- Frees the removed string.

#### `remove_var_index` (`env_remove.c:35-50`)
- Validates the array and index.
- Allocates the smaller replacement array.
- Rebuilds it with `copy_env_skip`.

#### `remove_env_index` (`env_remove.c:53-58`)
- Removes one live env entry and repoints global `environ`.

#### `remove_export_index` (`env_remove.c:61-65`)
- Removes one entry from the export table only.

### `env/env_utils.c`

#### `set_var_entry` (`env_utils.c:17-28`)
- Replaces an existing entry when the key already exists.
- Appends a new entry otherwise.

#### `count_env` (`env_utils.c:31-39`)
- Counts entries in any null-terminated env-style array.

#### `make_env_entry` (`env_utils.c:43-55`)
- Builds the exact `KEY=VALUE` string used by env storage.

#### `export_existing` (`env_utils.c:58-61`)
- Adds a bare key to `shell->exported` only if it is not already present.

#### `export_update` (`env_utils.c:64-80`)
- Builds one entry for `exported` and one duplicate for `env`.
- Upserts both tables.
- Repoints global `environ` at the live env array.

## 9) Evaluation-Critical Contracts

### Exit status
- `0`: success.
- `1`: generic runtime or builtin failure.
- `2`: syntax error or invalid numeric `exit`.
- `126`: found but not executable, or explicit directory path.
- `127`: command not found.
- `128 + signal`: child terminated by signal.

### Builtins in parent vs child
- Single-command `cd`, `export`, `unset`, and `exit` must affect the parent shell, so they run in the parent path.
- The same builtins inside a pipeline run in a child, so their state changes do not persist.

### Redirection precedence
- Pipe setup establishes default stdin/stdout for a stage.
- `apply_redirections` runs **after** that in pipeline children.
- Result: explicit redirections override pipe defaults.

### Memory ownership
- Parser allocates `t_cmd`, `args`, `quoted`, `redirs`, and legacy redirection strings.
- Executor may replace `args` and `quoted` during expansion.
- Prompt layer always frees the finished command list.

### Signal model
- Parent prompt: `SIGINT` redraws the prompt, `SIGQUIT` is ignored.
- Child command paths: `SIGINT` and `SIGQUIT` are reset to defaults.
- Heredoc uses its own temporary `SIGINT` handling.
