# Minishell Full Deep Dive

This guide is the companion to `MINISHELL_FULL_REFERENCE.md`.  
It focuses on the functions that are most likely to come up during evaluation and explains their control flow in the order the shell really uses them today.

## 1) The Real Runtime Story

When you type a line, the shell goes through these layers:

1. **Startup layer** prepares `t_shell`, duplicates the environment, increments `SHLVL`, and installs interactive signal handlers.
2. **Prompt layer** reads input, handles `Ctrl-C`, stores history, and splits the line into semicolon segments.
3. **Parser layer** rejects malformed syntax, splits by pipes, turns each stage into a `t_cmd`, and records redirections in order.
4. **Expansion layer** rebuilds `cmd->args` after `$VAR` and `$?` expansion.
5. **Execution layer** chooses builtin, external, or pipeline behavior.
6. **Cleanup layer** frees command structures and keeps only the final exit status.

The sections below walk the critical functions in that same order.

## 2) Data Model You Should Be Able to Explain

### `t_shell`
- `env`: environment actually passed to `execve`.
- `exported`: export table, including names that may not have values yet.
- `last_exit`: current value of `$?`.
- `should_exit`: loop break flag used by `exit`.
- `parser_status`: parser-side error channel.

### `t_cmd`
- Holds one pipeline stage.
- `args` and `quoted` store the token list and quote metadata.
- `redirs` preserves redirection order.
- Legacy fields like `infile`, `outfile`, `append`, and `heredoc_delim` are still synchronized for compatibility.

### `t_redir`
- One ordered redirection node.
- `quoted` matters because quoted redirection targets must not become ambiguous redirects after expansion.

### `t_pipe_exec`
- Temporary pipeline state shared across stages:
  - current node
  - pid list
  - previous read end
  - rightmost pid for final status

## 3) Startup and Prompt: How the Shell Stays Interactive

### `main` (`main.c:48-65`)

Line flow:
- `55-56`: `init_shell_state` duplicates the environment into two separate arrays: `env` and `exported`.
- `57-58`: shell statuses are initialized.
- `59`: `init_shlvl` updates `SHLVL` through the same env mutation path used by builtins.
- `60`: `setup_signals` installs parent-shell signal behavior.
- `61`: control moves into the REPL.
- `62-64`: the shell captures the final exit code and frees its persistent state.

Why this matters:
- The shell never works directly on `envp`; it owns its own copies.
- `SHLVL` is treated like a normal environment mutation, not a special case.

### `signal_handler` (`main.c:20-30`)

Line flow:
- `22`: store the incoming signal globally.
- `23-29`: only `SIGINT` triggers readline cleanup and redraw.

Why this matters:
- Prompt-time `Ctrl-C` should refresh the prompt without killing the shell.
- `SIGQUIT` is intentionally ignored in prompt mode.

### `prompt_loop` (`prompt.c:111-135`)

Line flow:
- `117`: `readline("$> ")` blocks until the user finishes a line.
- `118-121`: if the previous interaction was interrupted by `SIGINT`, the shell sets status `130` and clears the global marker.
- `122-123`: EOF returns `NULL`; minishell prints a newline and exits cleanly.
- `124-127`: empty lines are discarded.
- `129`: non-empty input goes into history.
- `130`: the whole line is handed to `execute_input_segments`.
- `131-133`: input is freed and the loop stops if `exit` requested shutdown.

Why this matters:
- The prompt layer owns line lifetime.
- History is added only for real commands, not for empty input.

### `execute_input_segments` (`prompt.c:86-108`)

Line flow:
- `91`: split the full line on unquoted semicolons.
- `92-98`: if splitting failed, promote parser status into `last_exit`.
- `100-104`: reject bad top-level syntax before execution starts.
- `106-107`: run all valid segments and free the split array.

Why this matters:
- Semicolon syntax errors are caught before building any `t_cmd`.
- Status propagation happens at the prompt boundary, not inside the executor.

### `check_segment` (`prompt.c:15-41`)

Line flow:
- `20`: trim spaces before checking visible syntax.
- `21-27`: validate normal non-empty segments.
  - `23-24`: reject leading `|`.
  - `25-26`: reject a segment ending with `|` when another semicolon segment follows.
- `29-33`: catch the case where a previous segment ends with `|` and the current segment is empty.
- `35-39`: distinguish legal trailing empties from real `;` or `;;` syntax errors.

Why this matters:
- This file owns semicolon-level structure checks.
- Pipe-level tokenization happens later, but obvious structural mistakes are rejected here.

## 4) Parsing: From Raw Text to `t_cmd`

### `split_semicolons` family (`parser_semicolons.c`)

#### `quote_state_changed` (`15-21`)
- Toggles quote state when opening or closing quoted text.

#### `fill_loop` (`32-52`)
- Scans the raw line once.
- When it sees an unquoted `;`, it copies everything from `start` to the delimiter.
- Then it moves `start` to the next character.

#### `fill_segments` (`55-76`)
- Starts with `start = 0` and `quote = 0`.
- Uses `fill_loop` to capture all middle segments.
- Rejects unmatched quotes after the scan.
- Copies the final trailing segment and null-terminates the result array.

#### `split_semicolons` (`79-93`)
- Sizes the output array from `parser_count_semicolons`.
- Allocates the list.
- Frees partial results if filling fails.

Why this matters:
- Semicolon splitting is quote-aware.
- That is why `echo "a;b"` stays one segment.

### `parse_command` (`parser.c:78-92`)

Line flow:
- `83`: clear old parser status.
- `84-85`: run quote-balance validation.
- `86-88`: split the segment into pipeline stages.
- `89-91`: parse each stage into a `t_cmd` node and link them.

Why this matters:
- `parse_command` is the real parser entry point for one semicolon segment.
- It never executes anything; it only builds data structures.

### `split_pipes` family (`parser_pipes.c`)

#### `count_pipes` (`15-36`)
- Counts only unquoted `|`.
- Returns `-1` if a quote never closes.

#### `push_segment` (`39-58`)
- Trims leading spaces inside a pipeline range.
- Rejects empty stages.
- Copies the substring into the result array.

#### `split_loop` (`78-104`)
- Tracks `start` and quote state.
- Saves a segment whenever an unquoted `|` is found.
- Saves the final segment after the scan.

#### `split_pipes` (`107-123`)
- Allocates enough slots for all stages plus `NULL`.
- Converts empty-stage cases into a pipe syntax error.

Why this matters:
- The shell rejects malformed pipelines early, before `parse_single`.
- Empty stages like `ls || wc` are caught here because one split range becomes empty.

### `parse_single` (`parser_single.c:86-108`)

Line flow:
- `91-92`: reject empty stage input.
- `93-97`: count tokens first and reject unmatched quotes.
- `99-100`: allocate the `t_cmd`.
- `102-106`: fill `args` and redirections; on failure free the partial command and report the syntax issue.

Why this matters:
- Allocation size comes from a counting pass first.
- The final `t_cmd` stores both normal arguments and ordered redirections.

### `fill_args` + `handle_word` (`parser_single.c:24-59`)

Line flow:
- `fill_args` scans left to right and skips spaces.
- `handle_word` decides what the current token means:
  - if it starts with `<` or `>`, parse a redirection
  - otherwise extract a word and store it in `args`
- `fill_args` null-terminates `cmd->args` at the end.

Why this matters:
- The parser does not build a separate AST here.
- A command is just a token array plus an ordered redirection list.

### `parser_extract_word` family (`parser_words.c`)

#### `get_word_len` (`15-36`)
- Calculates how large the final token buffer must be.
- Ignores quote characters themselves.
- Treats `<`, `>`, `<<`, and `>>` as standalone tokens.

#### `copy_quoted_content` (`52-62`)
- Copies only the inside of the quote pair.
- Returns the quote character so the caller remembers whether the token came from `'` or `"`.

#### `fill_word` (`65-84`)
- Builds the actual token.
- Removes quote delimiters.
- Stores the last quote type seen into `was_quoted`.

#### `parser_extract_word` (`87-104`)
- Allocates the token buffer.
- Resets `was_quoted`.
- Fills the token and returns it.

Why this matters:
- Quotes are removed during parsing, but quote type is not lost.
- Later, expansion uses that quote metadata to decide whether `$` is active.

### `parser_set_redirection` (`parser_redirections.c:86-105`)

Line flow:
- `94-96`: extract the operator token (`<`, `>`, `<<`, `>>`).
- `97`: skip spaces after the operator.
- `98-99`: if nothing remains, raise the unexpected-newline syntax error.
- `100-102`: extract the redirection target token and remember whether it was quoted.
- `103`: convert operator text into `t_redir_type`.
- `104`: free the temporary operator string.
- `105`: allocate and attach the redirection node.

Why this matters:
- Redirections are parsed during the token walk, not in a second pass.
- Quote information on the target is preserved for later ambiguous-redirect checks.

### `parser_add_redirection` + legacy sync (`parser_redirections.c:68-83`, `50-65`, `30-47`)

Line flow:
- Allocate `t_redir`.
- Store `type`, `target`, `quoted`, and `next`.
- Duplicate the target into legacy fields like `infile`, `outfile`, or `heredoc_delim`.
- Append the node to `cmd->redirs`.

Why this matters:
- The linked list preserves original order.
- The legacy fields let the rest of the shell keep older assumptions while the new ordered model exists too.

## 5) Expansion and Redirections: The Stage Before Execution

### `executor_expand_args` (`executor_expand.c:37-55`)

Line flow:
- `41-42`: allocate replacement arrays.
- `45-47`: prepare the build state.
- `47`: delegate the real scan to `fill_expanded_args`.
- `49-54`: install the rebuilt arrays back into the command.

Important detail:
- The command object is mutated in place.

### `fill_expanded_args` (`executor_expand.c:15-34`)

Line flow:
- Expand each original arg with `expand_argument`.
- Free the old arg immediately.
- Keep empty results only if the original token was quoted.
- Drop empty unquoted results.

Why this matters:
- This is the reason an unquoted `$EMPTY` can disappear while `"$EMPTY"` remains as an empty string argument.

### `expand_argument` (`executor_expand_arg.c:65-86`)

Line flow:
- `71-72`: if the token came from single quotes, duplicate it literally and skip expansion.
- `73`: start with an empty output string.
- `75-84`: walk the token character by character.
  - plain chars are appended literally
  - `$` is delegated to `expand_dollar`

### `expand_dollar` (`executor_expand_arg.c:44-62`)

Line flow:
- `48-54`: `$?` becomes the decimal form of `last_exit`.
- `56-60`: invalid variable starts keep `$` literally.
- `62`: valid names go through `expand_variable`.

### `expand_variable` (`executor_expand_arg.c:28-41`)

Line flow:
- Move past `$`.
- Scan the variable name with `is_var_char`.
- Extract the key.
- Fetch the env value with `get_env_value`.
- Append it to the growing output.

Why this matters:
- Expansion happens **after parsing** and **before execution**.
- The parser removes quotes; the executor decides whether `$` is still active.

### `apply_redirections` (`redirections.c:51-73`)

Line flow:
- Walk the redirection list in the same order the parser stored it.
- Expand the target first with `expand_redir_target`.
- Apply the low-level redirection with `apply_single_redir`.
- Free temporary expanded targets when needed.

Why this matters:
- Order is preserved, so `cmd >a >b` ends on `b`.
- Redirection expansion reuses the same variable-expansion engine as normal args.

### `expand_redir_target` (`redirections.c:25-36`)

Line flow:
- Heredoc delimiters are left untouched.
- Other targets are expanded.
- If an unquoted target becomes empty or contains spaces/tabs, the shell prints `ambiguous redirect` and aborts the command.

Why this matters:
- `>$FILE` is valid only if expansion resolves to exactly one pathname-like word.

### `redirect_heredoc` (`redirections_util.c:84-110`)

Line flow:
- `90-91`: create a pipe for heredoc content.
- `92-93`: install heredoc-specific signals.
- `94-103`: read lines until delimiter or interrupt.
- `104-105`: restore normal shell signals.
- `106-109`: connect the pipe's read end to stdin.

Why this matters:
- Heredoc is implemented as an in-memory pipe, not a temporary file.
- `Ctrl-C` during heredoc sets shell status `130`.

## 6) Executor: Dispatch, Forking, and Pipelines

### `execute_command` (`executor.c:94-106`)

Line flow:
- `96-97`: reject empty command objects.
- `98-99`: expand arguments first.
- `100-101`: if expansion erased every arg, treat it as a no-op success.
- `102-103`: if there is a `next` node, this is a pipeline.
- `104-105`: otherwise try the builtin path.
- `106`: fall back to external execution.

Why this matters:
- Expansion always happens before dispatch.
- Pipeline detection happens from the linked list, not from raw text.

### `execute_builtin` (`executor.c:32-53`)

Line flow:
- `37-38`: `exit` is handled first.
- `39-43`: reject non-builtins quickly.
- `44-46`: duplicate stdin and stdout.
- `49-50`: apply redirections directly in the parent.
- `51`: run the builtin.
- `52`: restore original stdio.

Why this matters:
- Parent-builtin execution is required for `cd`, `export`, `unset`, and `exit` to affect the shell itself.
- Because these run in the parent, redirections must be temporary.

### `execute_external_child` (`executor.c:56-75`)

Line flow:
- Reset child signals to defaults.
- Apply redirections.
- Resolve the executable path.
- Call `execve`.
- If execution still fails, map the error into a shell-style message and exit code.

Why this matters:
- Parent and child must not share the same signal semantics.

### `execute_external` (`executor.c:77-92`)

Line flow:
- Fork.
- Child runs `execute_external_child`.
- Parent waits once.
- Final status becomes either `WEXITSTATUS` or `128 + signal`.

Why this matters:
- Single external commands do not need the pipeline machinery.

### `execute_pipeline` (`executor_pipe.c:51-65`)

Line flow:
- Initialize `t_pipe_exec`.
- Loop stage by stage through `run_pipeline_step`.
- Close the final leftover `fd_in`.
- Wait every child.
- Free the pid array.

Why this matters:
- Pipeline state is centralized in one struct, not passed around as many loose variables.

### `run_pipeline_step` (`executor_pipe.c:24-48`)

Line flow:
- Expand the current stage args.
- Create a pipe if another stage follows.
- Fork one child.
- In parent:
  - store pid
  - update `last_pid`
  - close previous input fd
  - keep the new read end for the next stage

Why this matters:
- Each stage is expanded independently before it forks.

### `execute_pipeline_child` (`executor_pipe_utils.c:82-95`)

Line flow:
- Restore default signals.
- Wire stdin/stdout through `setup_pipeline_child`.
- Apply explicit command redirections.
- Dispatch execution through `run_pipeline_execve`.

Why this matters:
- The order is essential:
  1. pipe defaults
  2. explicit redirections
  3. exec

That is exactly why explicit redirections override pipe wiring.

### `run_pipeline_execve` (`executor_pipe_utils.c:26-51`)

Line flow:
- Empty stage: exit `0`.
- `exit`: run builtin logic in the child and exit with its status.
- Regular builtins inside pipelines: run them in the child and exit.
- Everything else: resolve path and call `execve`.

Why this matters:
- Builtins inside pipelines do not modify parent shell state.

### `wait_pipeline` (`executor_pipe_utils.c:63-79`)

Line flow:
- Wait every child pid.
- Update `last_exit` only when the pid matches `last_pid`.

Why this matters:
- Pipeline `$?` is the status of the **rightmost** stage, not the first one that finishes.

## 7) Builtins and Environment: Where Shell State Actually Changes

### `builtin_cd` (`cd_pwd.c:75-95`)

Line flow:
- Validate pointers.
- Resolve the target through `cd_get_target`.
- Capture old `PWD`.
- Call `chdir`.
- If successful, update `OLDPWD` and `PWD` through `cd_update_pwd`.

Why this matters:
- `cd` is one of the clearest examples of why some builtins must run in the parent shell.

### `cd_get_target` (`cd_utils.c:15-34`)

Line flow:
- Reject more than one argument.
- Use `args[1]` when present.
- Support a simple `$NAME` target lookup.
- Fall back to `HOME`.

### `resolve_cd_pwd` (`cd_path.c:97-114`)

Line flow:
- Join old logical `PWD` and target.
- Split on `/`.
- Remove empty parts and `.`.
- Collapse `..`.
- Rebuild the normalized absolute path.

Why this matters:
- This keeps `PWD` logical even if the user did not run `pwd -P`.

### `builtin_export` (`export.c:68-91`)

Line flow:
- `72-79`: no-arg mode prints the export table.
- `81-90`: validate every argument, report bad identifiers, and pass valid ones to `export_var`.

### `export_var` (`env.c:81-98`)

Line flow:
- Look for `=`.
- No `=`: treat as "export name".
- With `=`: split key/value and route to `export_update`.

### `export_update` (`env_utils.c:64-80`)

Line flow:
- Build one `KEY=VALUE` string for `exported`.
- Duplicate it for `env`.
- Upsert both arrays.
- Repoint global `environ`.

Why this matters:
- Minishell keeps two parallel views:
  - what is exported
  - what is currently passed to children

### `builtin_unset` (`unset.c:45-65`)

Line flow:
- Start with success.
- Validate every key.
- Report invalid keys but continue.
- Remove valid keys from both tables through `unset_key`.

### `builtin_exit` (`executor_exit.c:58-83`)

Line flow:
- No args: exit with current status.
- Invalid numeric arg: print error, set status `2`, and exit.
- Too many args: print error, keep the shell alive, set status `1`.
- One valid numeric arg: store the wrapped 8-bit code and exit.

Why this matters:
- This function combines parsing rules and control-flow rules.

### `init_shell_state` (`env_init.c:52-64`)

Line flow:
- Duplicate `envp` into `env`.
- Duplicate it again into `exported`.
- Clean up correctly if the second allocation fails.
- Point global `environ` at the live env table.

### `init_shlvl` (`env_init.c:67-82`)

Line flow:
- Read `SHLVL`.
- If numeric, increment it; otherwise reset to `1`.
- Write it back through `set_shell_env_var`.

Why this matters:
- Startup env handling already goes through the same mutation helpers as builtins, so behavior stays consistent.

## 8) The Contracts Evaluators Usually Ask About

### Why do `cd`, `export`, and `unset` run in the parent?
Because changing directories or environment variables in a child would disappear when the child exits. Single-command builtin execution therefore happens in the parent path.

### Why does only the last pipeline stage define `$?`?
Because `wait_pipeline` updates `shell->last_exit` only for `last_pid`, which is the rightmost stage.

### Why can redirection override a pipe?
Because `execute_pipeline_child` wires the default pipe fds first and then calls `apply_redirections`, so explicit command redirections replace the defaults.

### Why does unmatched quote fail before execution?
Because token boundaries are unreliable if quote state stays open. The parser rejects the line instead of guessing.

### Why keep both `redirs` and legacy fields?
`redirs` preserves the true ordered model, while legacy fields let existing execution code still ask simple questions like "what is the final outfile?".

## 9) Fast Oral Walkthrough for Evaluation

If someone asks you to explain the shell, this is the clean answer:

1. `main` initializes shell state, env copies, `SHLVL`, and signals.
2. `prompt_loop` reads one line and turns `Ctrl-C` into status `130`.
3. `split_semicolons` breaks the line into top-level segments while respecting quotes.
4. `check_segment` rejects bad `;` and `|` placement.
5. `parse_command` validates quotes, splits on pipes, and builds one `t_cmd` chain.
6. `executor_expand_args` performs `$VAR` and `$?` expansion using quote metadata.
7. `execute_command` chooses builtin, external, or pipeline execution.
8. `apply_redirections` runs in command order, and in pipelines it runs after pipe wiring.
9. `wait_pipeline` stores the last stage status into `$?`.
10. The prompt layer frees the whole command list and loops again.
