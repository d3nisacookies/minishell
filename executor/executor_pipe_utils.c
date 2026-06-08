/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/04 00:58:46 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	close_pipeline_fds(int *pipefd, t_pipe_exec *px, int has_next)
{
	if (px->fd_in != -1)
		close(px->fd_in);
	if (has_next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
}

static void	run_pipeline_execve(t_pipe_exec *px, t_shell *shell)
{
	char	*path;

	if (!px->cmd->args || !px->cmd->args[0])
		exit(0);
	if (ft_strcmp(px->cmd->args[0], "exit") == 0)
	{
		builtin_exit(shell, px->cmd);
		exit(shell->last_exit);
	}
	if (ft_strcmp(px->cmd->args[0], "echo") == 0 || ft_strcmp(px->cmd->args[0],
			"cd") == 0 || ft_strcmp(px->cmd->args[0], "pwd") == 0
		|| ft_strcmp(px->cmd->args[0], "export") == 0
		|| ft_strcmp(px->cmd->args[0], "unset") == 0)
	{
		run_regular_builtin(px->cmd, shell);
		exit(shell->last_exit);
	}
	path = resolve_command_path(shell, px->cmd->args[0]);
	if (path)
	{
		execve(path, px->cmd->args, shell->env);
		free(path);
	}
	executor_exit_exec_error(px->cmd->args[0]);
}

static void	setup_pipeline_child(t_pipe_exec *px, int *pipefd, int has_next)
{
	if (px->fd_in != -1)
		dup2(px->fd_in, STDIN_FILENO);
	if (has_next)
		dup2(pipefd[1], STDOUT_FILENO);
	close_pipeline_fds(pipefd, px, has_next);
}

void	wait_pipeline(pid_t *pids, int count, pid_t last_pid, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	while (i < count)
	{
		if (waitpid(pids[i], &status, 0) > 0 && pids[i] == last_pid)
		{
			if (WIFEXITED(status))
				shell->last_exit = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				shell->last_exit = 128 + WTERMSIG(status);
		}
		i++;
	}
}

void	execute_pipeline_child(t_pipe_exec *px, t_shell *shell, int *pipefd,
		int has_next)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_pipeline_child(px, pipefd, has_next);
	if (apply_redirections(px->cmd, shell) == -1)
	{
		if (g_signal == SIGINT)
			exit(130);
		exit(1);
	}
	run_pipeline_execve(px, shell);
}
