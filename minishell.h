/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** minishell header
*/

#ifndef MINISHELL_H
# define MINISHELL_H

# include "printf/ft_printf.h"
# include "printf/libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_signal;

/* ── Enums ── */
typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
	R_HEREDOC
}								t_redir_type;

/* ── Structs ── */
typedef struct s_redir
{
	t_redir_type				type;
	char						*target;
	struct s_redir				*next;
}								t_redir;

typedef struct s_cmd
{
	char						**args;
	int							*quoted;
	int							argc;
	char						*infile;
	char						*outfile;
	int							append;
	int							heredoc;
	char						*heredoc_delim;
	t_redir						*redirs;
	struct s_cmd				*next;
}								t_cmd;

typedef struct s_shell
{
	char						**env;
	int							last_exit;
	int						should_exit;
}								t_shell;

/* ── Prompt ── */
void							prompt_loop(t_shell *shell);

/* ── Parser ── */
t_cmd							*parse_command(char *input);
t_cmd							*parse_single(char *input);
char							**split_pipes(char *input);
int								parser_is_space(char c);
void							parser_skip_spaces(char *s, int *i);
void							parser_free_split(char **split, int count);
void							parser_put_unmatched_quote_error(void);
void							parser_put_pipe_error(void);
void							parser_set_status(int status);
int								parser_get_status(void);
int								parser_count_args(char *s);
char							*parser_extract_word(char *s, int *i,
									int *was_quoted);
int								parser_is_redirection(char *word);
int								parser_set_redirection(t_cmd *cmd, char *op,
									char *input, int *i);

/* ── Executor ── */
void							execute_command(t_cmd *cmd, t_shell *shell);
void							execute_pipeline(t_cmd *cmd, t_shell *shell);
int								have_next_pipe(t_cmd *cmd);
int								executor_expand_args(t_cmd *cmd,
									t_shell *shell);
void							executor_exit_exec_error(char *cmd_name);
void							free_cmd(t_cmd *cmd);
void							free_cmd_list(t_cmd *cmd);

/* ── Redirections ── */
int								apply_redirections(t_cmd *cmd);
int								redirect_input_file(char *path);
int								redirect_output_append(char *path);
int								redirect_output_truncate(char *path);
int								redirect_heredoc(char *delimiter);

/* ── Env ── */
char							**copy_env(char **envp);
int								count_env(char **envp);
int								find_env_index(char **env, char *key);
void							append_env(char ***env, char *new_entry);
char							*make_env_entry(char *key, char *value);
void							export_var(t_shell *shell, char *arg);
void							export_existing(t_shell *shell, char *arg);
void							export_update(t_shell *shell, char *key,
									char *value);
void							remove_env_index(t_shell *shell, int index);
void							copy_env_skip(char **new_env, char **old_env,
									int len, int skip);
char							*get_env_value(t_shell *shell, char *key);

/* ── Builtins ── */
void							builtin_export(t_shell *shell, t_cmd *cmd);
void							builtin_echo(t_shell *shell, t_cmd *cmd);
void							builtin_unset(t_shell *shell, t_cmd *cmd);
int								builtin_cd(t_shell *shell, t_cmd *cmd);
int								builtin_pwd(t_shell *shell);
int								cd_get_target(t_shell *shell, t_cmd *cmd,
									char **target);
int								set_shell_env_var(t_shell *shell, char *key,
									char *value);
int								cd_update_pwd(t_shell *shell, char *oldpwd);

/* ── Utils ── */
void							free_split_array(char **arr);
int								is_var_start(char c);
int								is_var_char(char c);
int								is_only_variable(char *str);
char							*ft_strtok(char *str, const char *delim);
char							**split_semicolons(char *input);
char							*trim_spaces(char *s);
int							starts_with_pipe(char *segment);
int							ends_with_pipe(char *segment);
int							is_trailing_empty_segment(char **segments, int index);

#endif