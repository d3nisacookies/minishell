/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 06:51:43 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

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

int	handle_builtin_redir_error(t_shell *shell, int saved_in, int saved_out)
{
	restore_stdio(saved_in, saved_out);
	if (g_signal != SIGINT)
		shell->last_exit = 1;
	return (1);
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
