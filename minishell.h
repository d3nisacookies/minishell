/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** minishell header
*/

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

extern volatile sig_atomic_t g_signal;

/* Structures */
typedef struct s_cmd
{
	char **args;
	int argc;
} t_cmd;

/* Function prototypes */
void	prompt_loop(void);
t_cmd	*parse_command(char *input);
void	execute_command(t_cmd *cmd);
void	free_cmd(t_cmd *cmd);

#endif /* !MINISHELL_H */
