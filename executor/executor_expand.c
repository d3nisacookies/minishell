/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:14:47 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:14:48 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	fill_expanded_args(t_cmd *cmd, t_shell *shell, t_expand_build *b)
{
	char	*expanded;

	while (b->i < cmd->argc)
	{
		expanded = expand_argument(shell, cmd->args[b->i], cmd->quoted[b->i]);
		if (!expanded)
			return (-1);
		free(cmd->args[b->i]);
		if (expanded[0] || cmd->quoted[b->i])
		{
			b->new_args[b->count] = expanded;
			b->new_quoted[b->count++] = cmd->quoted[b->i];
		}
		else
			free(expanded);
		b->i++;
	}
	return (0);
}

int	executor_expand_args(t_cmd *cmd, t_shell *shell)
{
	t_expand_build	b;

	b.new_args = malloc(sizeof(char *) * (cmd->argc + 1));
	b.new_quoted = malloc(sizeof(int) * (cmd->argc + 1));
	if (!b.new_args || !b.new_quoted)
		return (free(b.new_args), free(b.new_quoted), -1);
	b.i = 0;
	b.count = 0;
	if (fill_expanded_args(cmd, shell, &b) == -1)
		return (free(b.new_args), free(b.new_quoted), -1);
	b.new_args[b.count] = NULL;
	free(cmd->args);
	free(cmd->quoted);
	cmd->args = b.new_args;
	cmd->quoted = b.new_quoted;
	cmd->argc = b.count;
	return (0);
}
