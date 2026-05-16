/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** minishell header
*/

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include "printf/ft_printf.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stddef.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_signal;

typedef struct s_cmd
{
	char			**args;
	int				argc;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_shell
{
	char	**env;
	int		last_exit;
}	t_shell;

void	prompt_loop(t_shell *shell);
t_cmd	*parse_command(char *input);
t_cmd	*parse_single(char *input);
char	**split_pipes(char *input);
int		parser_is_space(char c);
void	parser_skip_spaces(char *s, int *i);
void	parser_free_split(char **split, int count);
void	parser_put_unmatched_quote_error(void);
void	parser_put_pipe_error(void);
int		parser_count_args(char *s);
char	*parser_extract_word(char *s, int *i);
void	execute_command(t_cmd *cmd, t_shell *shell);
void	free_cmd(t_cmd *cmd);
char	**copy_env(char **envp);
char	*make_env_entry(char *key, char *value);
void	append_env(char ***env, char *new_entry);
int		find_env_index(char **env, char *key);
void	export_var(t_shell *shell, char *arg);
void	builtin_export(t_shell *shell, t_cmd *cmd);
void	builtin_echo(t_shell *shell, t_cmd *cmd);
char	*ft_strtok(char *str, const char *delim);

#endif