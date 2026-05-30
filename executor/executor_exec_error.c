/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_exec_error.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:13:36 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:13:37 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

static int	is_directory_path(char *cmd_name)
{
	struct stat	st;

	if (!cmd_name)
		return (0);
	if (!ft_strchr(cmd_name, '/'))
		return (0);
	if (stat(cmd_name, &st) != 0)
		return (0);
	return (S_ISDIR(st.st_mode));
}

void	executor_exit_exec_error(char *cmd_name)
{
	if (is_directory_path(cmd_name))
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		exit(126);
	}
	if (errno == ENOENT)
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	if (errno == EACCES)
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		exit(126);
	}
	perror(cmd_name);
	exit(1);
}
