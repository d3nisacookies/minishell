/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:13:59 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:08:46 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	execute_redirections_only(t_cmd *cmd, t_shell *shell)
{
	int	saved_in;
	int	saved_out;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1)
		return (perror("dup"), restore_stdio(saved_in, saved_out),
			shell->last_exit = 1, 1);
	if (apply_redirections(cmd, shell) == -1)
		return (handle_builtin_redir_error(shell, saved_in, saved_out));
	restore_stdio(saved_in, saved_out);
	shell->last_exit = 0;
	return (1);
}

int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	saved_in;
	int	saved_out;

	if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(shell, cmd), 1);
	if (ft_strcmp(cmd->args[0], "echo") != 0 && ft_strcmp(cmd->args[0],
			"cd") != 0 && ft_strcmp(cmd->args[0], "pwd") != 0
		&& ft_strcmp(cmd->args[0], "export") != 0 && ft_strcmp(cmd->args[0],
			"unset") != 0)
		return (0);
	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1)
		return (perror("dup"), restore_stdio(saved_in, saved_out),
			shell->last_exit = 1, 1);
	if (apply_redirections(cmd, shell) == -1)
		return (handle_builtin_redir_error(shell, saved_in, saved_out));
	run_regular_builtin(cmd, shell);
	restore_stdio(saved_in, saved_out);
	return (1);
}

void	execute_external_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (apply_redirections(cmd, shell) == -1)
	{
		if (g_signal == SIGINT)
			exit(130);
		exit(1);
	}
	path = resolve_command_path(shell, cmd->args[0]);
	if (path)
	{
		executor_execve_with_fallback(path, cmd->args, shell->env);
		free(path);
	}
	executor_exit_exec_error(cmd->args[0]);
}

void	execute_external(t_cmd *cmd, t_shell *shell)
{
	pid_t				pid;
	int					status;
	struct sigaction	old_int;
	struct sigaction	old_quit;

	if (ignore_shell_signals(&old_int, &old_quit) == -1)
		return ((void)(shell->last_exit = 1));
	pid = fork();
	if (pid == -1)
	{
		restore_shell_signals(&old_int, &old_quit);
		return (perror("fork"), (void)(shell->last_exit = 1));
	}
	if (pid == 0)
		execute_external_child(cmd, shell);
	waitpid(pid, &status, 0);
	restore_shell_signals(&old_int, &old_quit);
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		shell->last_exit = 128 + WTERMSIG(status);
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
