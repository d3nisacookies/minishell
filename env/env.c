/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 16:08:18 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 16:08:20 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"

char	**copy_env(char **envp)
{
	char	**env;
	int		len;
	int		i;

	len = count_env(envp);
	env = malloc(sizeof(char *) * (len + 1));
	if (!env)
		return (NULL);
	i = 0;
	while (i < len)
	{
		env[i] = ft_strdup(envp[i]);
		if (!env[i])
			return (NULL);
		i++;
	}
	env[i] = NULL;
	return (env);
}

int	find_env_index(char **env, char *key)
{
	int	i;
	int	key_len;

	if (!env || !key)
		return (-1);
	key_len = ft_strlen(key);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0 && (env[i][key_len] == '='
			|| env[i][key_len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

void	append_env(char ***env, char *new_entry)
{
	char	**new_env;
	int		len;
	int		i;

	if (!env || !new_entry)
		return ;
	len = count_env(*env);
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return ;
	i = 0;
	while (i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(*env);
	*env = new_env;
}

void	export_var(t_shell *shell, char *arg)
{
	char	*equals;
	char	*key;

	if (!shell || !arg)
		return ;
	equals = ft_strchr(arg, '=');
	if (!equals)
	{
		export_existing(shell, arg);
		return ;
	}
	key = ft_substr(arg, 0, equals - arg);
	if (!key)
		return ;
	export_update(shell, key, equals + 1);
	free(key);
}
