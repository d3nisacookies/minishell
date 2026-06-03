/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/04 01:17:09 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

// static int	is_nonexec_file(char *cmd_name)
// {
// 	struct stat	st;

// 	if (stat(cmd_name, &st) != 0)
// 		return (0);
// 	if (S_ISDIR(st.st_mode))
// 		return (0);
// 	return (access(cmd_name, X_OK) != 0);
// }

char	*resolve_command_path(t_shell *shell, char *cmd_name)
{
	char	*path;

	if (!cmd_name || !*cmd_name)
		return (NULL);
	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	path = resolve_path_from_env(shell, cmd_name);
	if (path || errno != ENOENT)
		return (path);
	return (NULL);
}

void	restore_stdio(int saved_in, int saved_out)
{
	if (saved_in != -1)
	{
		dup2(saved_in, STDIN_FILENO);
		close(saved_in);
	}
	if (saved_out != -1)
	{
		dup2(saved_out, STDOUT_FILENO);
		close(saved_out);
	}
}
