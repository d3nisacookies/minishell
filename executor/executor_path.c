/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/04 01:13:57 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_command_path(char *dir, char *cmd)
{
	char	*tmp;

	if (!dir || dir[0] == '\0')
		return (ft_strdup(cmd));
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	cmd = ft_strjoin(tmp, cmd);
	free(tmp);
	return (cmd);
}

static int	path_is_executable(char *dir, char *cmd_name, char **full_path)
{
	*full_path = join_command_path(dir, cmd_name);
	if (!*full_path)
		return (0);
	if (access(*full_path, X_OK) == 0)
		return (1);
	free(*full_path);
	*full_path = NULL;
	return (0);
}

char	*resolve_path_from_env(t_shell *shell, char *cmd_name)
{
	char	**paths;
	char	*path_var;
	char	*full_path;
	int		index;

	path_var = get_env_value(shell, "PATH");
	if (!path_var)
		return (ft_strdup(cmd_name));
	paths = ft_split(path_var, ':');
	if (!paths)
		return (NULL);
	index = 0;
	while (paths[index])
	{
		if (path_is_executable(paths[index], cmd_name, &full_path))
			return (free_split_array(paths), full_path);
		index++;
	}
	free_split_array(paths);
	errno = ENOENT;
	return (NULL);
}
