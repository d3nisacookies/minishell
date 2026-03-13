/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** command executor
*/

#include "minishell.h"

void	execute_command(t_cmd *cmd)
{
	pid_t	pid;
	int	status;

	if (strcmp(cmd->args[0], "exit") == 0)
		exit(0);
	if (strcmp(cmd->args[0], "cd") == 0)
	{
		if (cmd->argc > 1)
			chdir(cmd->args[1]);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
	{
		execvp(cmd->args[0], cmd->args);
		perror(cmd->args[0]);
		exit(1);
	}
	else
		waitpid(pid, &status, 0);
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (cmd == NULL)
		return ;
	i = 0;
	while (cmd->args[i] != NULL)
	{
		free(cmd->args[i]);
		i++;
	}
	free(cmd->args);
	free(cmd);
}
