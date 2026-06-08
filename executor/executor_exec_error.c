/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_exec_error.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:13:36 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:11:50 by akaung           ###   ########.fr       */
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

void	execute_command(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args)
		return ;
	if (executor_expand_args(cmd, shell) == -1)
		return ((void)(shell->last_exit = 1));
	if (!cmd->args[0])
	{
		execute_redirections_only(cmd, shell);
		return ;
	}
	if (cmd->next)
		return (execute_pipeline(cmd, shell));
	if (execute_builtin(cmd, shell))
		return ;
	execute_external(cmd, shell);
}
