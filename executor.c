/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** command executor
*/

#include "minishell.h"


extern char **environ;

static void	exit_exec_error(char *cmd_name)
{
	if (errno == ENOENT)
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	if (errno == EACCES)
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		exit(126);
	}
	perror(cmd_name);
	exit(1);
}

static void	restore_stdio(int saved_in, int saved_out)
{
	if (saved_in != -1)
	{
		dup2(saved_in, STDIN_FILENO);
		close(saved_in);
	}
	if (saved_out != -1)
	{
		dup2(saved_out, STDOUT_FILENO);
		close(saved_out);
	}
}

static int	prepare_builtin_redirs(t_cmd *cmd, int *saved_in, int *saved_out)
{
	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	if (*saved_in == -1 || *saved_out == -1)
	{
		perror("dup");
		restore_stdio(*saved_in, *saved_out);
		return (-1);
	}
	if (apply_redirections(cmd) == -1)
	{
		restore_stdio(*saved_in, *saved_out);
		return (-1);
	}
	return (0);
}

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
		int	saved_in;
		int	saved_out;

		if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		{
			shell->last_exit = 1;
			return ;
		}
			builtin_echo(shell, cmd);
			restore_stdio(saved_in, saved_out);
			shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "cd") == 0)
	{
		shell->last_exit = builtin_cd(shell, cmd);
		return ;
	}
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
	{
		shell->last_exit = builtin_pwd(shell);
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
		if (apply_redirections(cmd) == -1)
			exit(1);
		execvp(cmd->args[0], cmd->args);
		exit_exec_error(cmd->args[0]);
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
	free(cmd->quoted);
	free(cmd->infile);
	free(cmd->outfile);
	free(cmd);
}
