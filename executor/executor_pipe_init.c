/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_init.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/04 00:58:39 by akaung           ###   ########.fr       */
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
