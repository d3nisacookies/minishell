/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 00:00:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:11:45 by akaung           ###   ########.fr       */
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

static char	**build_shell_argv(char *path, char **args)
{
	char	**argv;
	int		i;
	int		argc;

	argc = 0;
	while (args && args[argc])
		argc++;
	argv = malloc(sizeof(char *) * (argc + 2));
	if (!argv)
		return (errno = ENOMEM, NULL);
	argv[0] = "sh";
	argv[1] = path;
	i = 1;
	while (i < argc)
	{
		argv[i + 1] = args[i];
		i++;
	}
	argv[argc + 1] = NULL;
	return (argv);
}

int	executor_execve_with_fallback(char *path, char **args, char **env)
{
	char	**shell_argv;

	execve(path, args, env);
	if (errno != ENOEXEC)
		return (-1);
	shell_argv = build_shell_argv(path, args);
	if (!shell_argv)
		return (-1);
	execve("/bin/sh", shell_argv, env);
	free(shell_argv);
	return (-1);
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
