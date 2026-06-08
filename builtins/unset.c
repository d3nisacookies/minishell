/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 14:27:31 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 14:27:32 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_identifier(char *key)
{
	int	i;

	if (!key || key[0] == '\0')
		return (0);
	if (!(ft_isalpha(key[0]) || key[0] == '_'))
		return (0);
	i = 1;
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static void	unset_key(t_shell *shell, char *key)
{
	int		idx;

	idx = find_env_index(shell->env, key);
	if (idx != -1)
		remove_env_index(shell, idx);
	idx = find_env_index(shell->exported, key);
	if (idx != -1)
		remove_export_index(shell, idx);
}

void	builtin_unset(t_shell *shell, t_cmd *cmd)
{
	int		i;
	char	*key;

	if (!shell || !cmd || !cmd->args)
		return ;
	shell->last_exit = 0;
	i = 1;
	while (i < cmd->argc)
	{
		key = cmd->args[i++];
		if (!is_valid_identifier(key))
		{
			ft_putstr_fd("unset: not a valid identifier: ", 2);
			ft_putendl_fd(key, 2);
			shell->last_exit = 1;
			continue ;
		}
		unset_key(shell, key);
	}
}
