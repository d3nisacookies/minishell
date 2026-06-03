/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/03 00:00:00 by akaung           ###   ########.fr       */
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

char	*resolve_command_path(t_shell *shell, char *cmd_name)
{
	char	**paths;
	char	*path_var;
	char	*full_path;
	int		index;

	if (!cmd_name || !*cmd_name)
		return (NULL);
	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	path_var = get_env_value(shell, "PATH");
	if (!path_var)
		return (ft_strdup(cmd_name));
	paths = ft_split(path_var, ':');
	if (!paths)
		return (NULL);
	index = 0;
	while (paths[index])
	{
		full_path = join_command_path(paths[index], cmd_name);
		if (full_path && access(full_path, X_OK) == 0)
			return (free_split_array(paths), full_path);
		free(full_path);
		index++;
	}
	free_split_array(paths);
	return (ft_strdup(cmd_name));
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

void	run_regular_builtin(t_cmd *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		builtin_echo(shell, cmd);
		shell->last_exit = 0;
	}
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		shell->last_exit = builtin_cd(shell, cmd);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		shell->last_exit = builtin_pwd(shell);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		builtin_export(shell, cmd);
	else
		builtin_unset(shell, cmd);
}