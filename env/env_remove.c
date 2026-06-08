/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_remove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 13:58:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/08 13:58:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

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

static void	remove_var_index(char ***vars, int index)
{
	char	**new_vars;
	int		len;

	if (!vars || !*vars || index < 0)
		return ;
	len = count_env(*vars);
	if (index >= len)
		return ;
	new_vars = malloc(sizeof(char *) * len);
	if (!new_vars)
		return ;
	copy_env_skip(new_vars, *vars, len, index);
	free(*vars);
	*vars = new_vars;
}

void	remove_env_index(t_shell *shell, int index)
{
	if (!shell)
		return ;
	remove_var_index(&shell->env, index);
	environ = shell->env;
}

void	remove_export_index(t_shell *shell, int index)
{
	if (!shell)
		return ;
	remove_var_index(&shell->exported, index);
}
