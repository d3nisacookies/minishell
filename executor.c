/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** command executor
*/

#include "minishell.h"

extern char	**environ;

void	execute_command(t_cmd *cmd, t_shell *shell)
{
	pid_t pid;
	int status;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (cmd->next)
	{
		execute_pipeline(cmd, shell);
		return ;
	}
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		exit(0);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(shell, cmd);
		shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "cd") == 0)
	{
		if (cmd->argc > 1)
		{
			if (chdir(cmd->args[1]) == -1)
			{
				perror("cd");
				shell->last_exit = 1;
				return ;
			}
		}
		shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "export") == 0)
	{
		builtin_export(shell, cmd);
		shell->last_exit = 0;
		return ;
	}
	environ = shell->env;
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		shell->last_exit = 1;
		return ;
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(cmd->args[0], cmd->args);
		perror(cmd->args[0]);
		exit(1);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			shell->last_exit = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->last_exit = 128 + WTERMSIG(status);
	}
}

void	free_cmd(t_cmd *cmd)
{
	int i;

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
