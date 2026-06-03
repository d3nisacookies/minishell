/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/03 00:00:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_pipeline_cmds(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
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
	char	*path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (px->fd_in != -1)
		dup2(px->fd_in, STDIN_FILENO);
	if (has_next)
		dup2(pipefd[1], STDOUT_FILENO);
	if (px->fd_in != -1)
		close(px->fd_in);
	if (has_next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
	if (apply_redirections(px->cmd) == -1)
		exit(1);
	if (!px->cmd->args || !px->cmd->args[0])
		exit(0);
	path = resolve_command_path(shell, px->cmd->args[0]);
	if (path)
	{
		execve(path, px->cmd->args, shell->env);
		free(path);
	}
	executor_exit_exec_error(px->cmd->args[0]);
}

int	init_pipeline_exec(t_pipe_exec *px, t_cmd *cmd, t_shell *shell)
{
	int	count;

	count = count_pipeline_cmds(cmd);
	px->pids = malloc(sizeof(pid_t) * count);
	if (!px->pids)
		return (shell->last_exit = 1, -1);
	px->idx = 0;
	px->fd_in = -1;
	px->last_pid = -1;
	px->cmd = cmd;
	return (0);
}