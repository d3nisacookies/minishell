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

static void	set_var_entry(char ***vars, char *key, char *new_entry)
{
	int	idx;

	idx = find_env_index(*vars, key);
	if (idx != -1)
	{
		free((*vars)[idx]);
		(*vars)[idx] = new_entry;
	}
	else
		append_env(vars, new_entry);
}

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
	if (find_env_index(shell->exported, arg) == -1)
		append_env(&shell->exported, ft_strdup(arg));
}

void	export_update(t_shell *shell, char *key, char *value)
{
	char	*export_entry;
	char	*env_entry;

	export_entry = make_env_entry(key, value);
	if (!export_entry)
		return ;
	env_entry = ft_strdup(export_entry);
	if (!env_entry)
	{
		free(export_entry);
		return ;
	}
	set_var_entry(&shell->exported, key, export_entry);
	set_var_entry(&shell->env, key, env_entry);
	environ = shell->env;
}
