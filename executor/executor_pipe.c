/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:15:04 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:16:30 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

static void	close_pipe_pair(int *pipefd, int has_next)
{
	if (has_next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
}

static void	wait_pipeline(pid_t *pids, int count, pid_t last_pid,
		t_shell *shell)
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

static void	exec_pipeline_child(t_pipe_exec *px, t_shell *shell, int *pipefd,
		int has_next)
{
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
	environ = shell->env;
	execvp(px->cmd->args[0], px->cmd->args);
	executor_exit_exec_error(px->cmd->args[0]);
}

static int	run_pipeline_step(t_pipe_exec *px, t_shell *shell)
{
	int		pipefd[2];
	int		has_next;
	pid_t	pid;

	if (executor_expand_args(px->cmd, shell) == -1)
		return (shell->last_exit = 1, -1);
	has_next = (px->cmd->next != NULL);
	if (has_next && pipe(pipefd) == -1)
		return (perror("pipe"), shell->last_exit = 1, -1);
	pid = fork();
	if (pid == -1)
		return (close_pipe_pair(pipefd, has_next), perror("fork"),
			shell->last_exit = 1, -1);
	if (pid == 0)
		exec_pipeline_child(px, shell, pipefd, has_next);
	px->pids[px->idx++] = pid;
	px->last_pid = pid;
	if (px->fd_in != -1)
		close(px->fd_in);
	if (has_next)
		return (close(pipefd[1]), px->fd_in = pipefd[0], 0);
	px->fd_in = -1;
	return (0);
}

void	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
	t_pipe_exec	px;
	int			count;

	if (!cmd)
		return ;
	count = 0;
	px.cmd = cmd;
	while (px.cmd && ++count)
		px.cmd = px.cmd->next;
	px.pids = malloc(sizeof(pid_t) * count);
	if (!px.pids)
		return ((void)(shell->last_exit = 1));
	px.idx = 0;
	px.fd_in = -1;
	px.last_pid = -1;
	px.cmd = cmd;
	while (px.cmd && run_pipeline_step(&px, shell) == 0)
		px.cmd = px.cmd->next;
	if (px.fd_in != -1)
		close(px.fd_in);
	if (px.idx > 0)
		wait_pipeline(px.pids, px.idx, px.last_pid, shell);
	free(px.pids);
}
