/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 14:28:21 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 16:00:17 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

int	count_env(char **envp)
{
	int	i;

	if (!envp)
		return (0);
	i = 0;
	while (envp[i])
		i++;
	return (i);
}

char	*make_env_entry(char *key, char *value)
{
	char	*tmp;
	char	*entry;

	if (!key || !value)
		return (NULL);
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

void	export_existing(t_shell *shell, char *arg)
{
	if (find_env_index(shell->env, arg) == -1)
	{
		append_env(&shell->env, ft_strdup(arg));
		environ = shell->env;
	}
}

void	export_update(t_shell *shell, char *key, char *value)
{
	char	*new_entry;
	int		idx;

	new_entry = make_env_entry(key, value);
	if (!new_entry)
		return ;
	idx = find_env_index(shell->env, key);
	if (idx != -1)
	{
		free(shell->env[idx]);
		shell->env[idx] = new_entry;
	}
	else
		append_env(&shell->env, new_entry);
	environ = shell->env;
}

void	copy_env_skip(char **new_env, char **old_env, int len, int skip)
{
	int	old_i;
	int	new_i;

	old_i = 0;
	new_i = 0;
	while (old_i < len)
	{
		if (old_i == skip)
			free(old_env[old_i]);
		else
			new_env[new_i++] = old_env[old_i];
		old_i++;
	}
	new_env[new_i] = NULL;
}
