/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 18:08:35 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 18:09:36 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ambiguous_redirect(t_shell *shell, char *target, char *expanded)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(target, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	free(expanded);
	shell->last_exit = 1;
	return (-1);
}

static int	expand_redir_target(t_redir *redir, t_shell *shell, char **target)
{
	*target = redir->target;
	if (redir->type == R_HEREDOC)
		return (0);
	*target = expand_argument(shell, redir->target, redir->quoted);
	if (!*target)
		return (-1);
	if (!redir->quoted && (!(*target)[0] || ft_strchr(*target, ' ')
		|| ft_strchr(*target, '\t')))
		return (ambiguous_redirect(shell, redir->target, *target));
	return (1);
}

static int	apply_single_redir(t_redir *redir, t_shell *shell, char *target)
{
	if (redir->type == R_IN)
		return (redirect_input_file(target));
	if (redir->type == R_OUT)
		return (redirect_output_truncate(target));
	if (redir->type == R_APPEND)
		return (redirect_output_append(target));
	return (redirect_heredoc(target, shell, redir->quoted));
	return (0);
}

int	apply_redirections(t_cmd *cmd, t_shell *shell)
{
	t_redir	*current;
	char	*target;
	int		needs_free;

	current = cmd->redirs;
	while (current)
	{
		needs_free = expand_redir_target(current, shell, &target);
		if (needs_free == -1)
			return (-1);
		if (apply_single_redir(current, shell, target) == -1)
		{
			if (needs_free)
				free(target);
			return (-1);
		}
		if (needs_free)
			free(target);
		current = current->next;
	}
	return (0);
}
