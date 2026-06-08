/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 14:13:27 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 14:17:52 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	set_shell_env_var(t_shell *shell, char *key, char *value)
{
	char	*tmp;
	char	*entry;

	if (!shell || !key || !value)
		return (1);
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (1);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!entry)
		return (1);
	export_var(shell, entry);
	free(entry);
	return (0);
}

char	*get_env_value(t_shell *shell, char *key)
{
	int		idx;
	char	*equals;

	if (!shell || !key || key[0] == '\0')
		return (NULL);
	idx = find_env_index(shell->env, key);
	if (idx != -1)
	{
		equals = ft_strchr(shell->env[idx], '=');
		if (!equals)
			return ("");
		return (equals + 1);
	}
	if (find_env_index(shell->exported, key) != -1)
		return ("");
	return (NULL);
}

int	builtin_pwd(t_shell *shell)
{
	char	*cwd;

	cwd = get_env_value(shell, "PWD");
	if (cwd && cwd[0] != '\0')
	{
		ft_printf("%s\n", cwd);
		return (0);
	}
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return (1);
	}
	ft_printf("%s\n", cwd);
	free(cwd);
	return (0);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*target;
	char	*oldpwd;

	if (!shell || !cmd || !cmd->args)
		return (1);
	if (cd_get_target(shell, cmd, &target))
		return (1);
	oldpwd = get_env_value(shell, "PWD");
	if (oldpwd)
		oldpwd = ft_strdup(oldpwd);
	else
		oldpwd = getcwd(NULL, 0);
	if (chdir(target) == -1)
	{
		perror("cd");
		free(oldpwd);
		return (1);
	}
	return (cd_update_pwd(shell, oldpwd, target));
}
