/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:15:04 by akaung            #+#    #+#             */
/*   Updated: 2026/06/03 00:00:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	close_pipe_pair(int *pipefd, int has_next)
{
	if (has_next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
}

static int	run_pipeline_step(t_pipe_exec *px, t_shell *shell)
{
	int		pipefd[2];
	int		has_next;
	pid_t	pid;

	if (executor_expand_args(px->cmd, shell) == -1)
		return (shell->last_exit = 0, -1);
	has_next = (px->cmd->next != NULL);
	if (has_next && pipe(pipefd) == -1)
		return (perror("pipe"), shell->last_exit = 1, -1);
	pid = fork();
	if (pid == -1)
		return (close_pipe_pair(pipefd, has_next), perror("fork"),
			shell->last_exit = 1, -1);
	if (pid == 0)
		execute_pipeline_child(px, shell, pipefd, has_next);
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
	t_pipe_exec			px;
	struct sigaction	old_int;
	struct sigaction	old_quit;

	if (!cmd)
		return ;
	if (ignore_shell_signals(&old_int, &old_quit) == -1)
		return ((void)(shell->last_exit = 1));
	if (init_pipeline_exec(&px, cmd, shell) == -1)
	{
		restore_shell_signals(&old_int, &old_quit);
		return ;
	}
	while (px.cmd && run_pipeline_step(&px, shell) == 0)
		px.cmd = px.cmd->next;
	if (px.fd_in != -1)
		close(px.fd_in);
	if (px.idx > 0)
		wait_pipeline(px.pids, px.idx, px.last_pid, shell);
	free(px.pids);
	restore_shell_signals(&old_int, &old_quit);
}
