/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 13:58:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/08 13:58:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

static void	free_env_copy(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

static int	is_numeric_value(char *value)
{
	int	i;

	if (!value || value[0] == '\0')
		return (0);
	i = 0;
	if (value[i] == '+' || value[i] == '-')
		i++;
	if (value[i] == '\0')
		return (0);
	while (value[i])
	{
		if (!ft_isdigit(value[i]))
			return (0);
		i++;
	}
	return (1);
}

int	init_shell_state(t_shell *shell, char **envp)
{
	shell->env = copy_env(envp);
	if (!shell->env)
		return (1);
	shell->exported = copy_env(envp);
	if (!shell->exported)
	{
		free_env_copy(shell->env);
		return (1);
	}
	environ = shell->env;
	return (0);
}

void	init_shlvl(t_shell *shell)
{
	char	*current;
	char	*new_value;
	int		level;

	current = get_env_value(shell, "SHLVL");
	if (is_numeric_value(current))
		level = ft_atoi(current) + 1;
	else
		level = 1;
	new_value = ft_itoa(level);
	if (!new_value)
		return ;
	set_shell_env_var(shell, "SHLVL", new_value);
	free(new_value);
}

void	free_shell_state(t_shell *shell)
{
	if (!shell)
		return ;
	free_env_copy(shell->env);
	free_env_copy(shell->exported);
}
