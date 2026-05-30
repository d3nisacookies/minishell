/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 13:27:14 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 14:17:41 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	cd_get_target(t_shell *shell, t_cmd *cmd, char **target)
{
	if (cmd->argc > 2)
	{
		ft_putstr_fd("cd : too many arguments\n", 2);
		return (1);
	}
	*target = cmd->args[1];
	if (*target && (*target)[0] == '$' && (*target)[1] != '\0')
		*target = get_env_value(shell, *target + 1);
	if (!*target)
	{
		*target = get_env_value(shell, "HOME");
		if (!*target || (*target)[0] == '\0')
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (1);
		}
	}
	return (0);
}

int	cd_update_pwd(t_shell *shell, char *oldpwd)
{
	char	*newpwd;

	newpwd = getcwd(NULL, 0);
	if (oldpwd)
	{
		set_shell_env_var(shell, "OLDPWD", oldpwd);
		free(oldpwd);
	}
	if (newpwd)
	{
		set_shell_env_var(shell, "PWD", newpwd);
		free(newpwd);
	}
	return (0);
}
