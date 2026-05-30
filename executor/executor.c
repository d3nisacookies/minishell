#include "minishell.h"

extern char	**environ;

static int	get_exit_sign(char *s, int *i, int *negative,
		unsigned long long *limit)
{
	*i = 0;
	*negative = 0;
	*limit = (unsigned long long)LLONG_MAX;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
		{
			*negative = 1;
			*limit = (unsigned long long)LLONG_MAX + 1;
		}
		(*i)++;
	}
	return (s[*i] != '\0');
}

static int	parse_exit_code(char *s, unsigned char *code)
{
	unsigned long long	value;
	unsigned long long	limit;
	int					negative;
	int					i;

	if (!get_exit_sign(s, &i, &negative, &limit))
		return (0);
	value = 0;
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		if (value > (limit - (s[i] - '0')) / 10)
			return (0);
		value = value * 10 + (s[i] - '0');
		i++;
	}
	if (negative)
		*code = (unsigned char)(0 - value);
	else
		*code = (unsigned char)value;
	return (1);
}

static void	builtin_exit(t_shell *shell, t_cmd *cmd)
{
	unsigned char	code;

	if (cmd->argc == 1)
		exit(shell->last_exit);
	if (!parse_exit_code(cmd->args[1], &code))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		exit(2);
	}
	if (cmd->argc > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		shell->last_exit = 1;
		return ;
	}
	exit(code);
}

static void	restore_stdio(int saved_in, int saved_out)
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

static int	prepare_builtin_redirs(t_cmd *cmd, int *saved_in, int *saved_out)
{
	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	if (*saved_in == -1 || *saved_out == -1)
	{
		perror("dup");
		restore_stdio(*saved_in, *saved_out);
		return (-1);
	}
	if (apply_redirections(cmd) == -1)
	{
		restore_stdio(*saved_in, *saved_out);
		return (-1);
	}
	return (0);
}

static int	execute_builtin(t_cmd *cmd, t_shell *shell)
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
	if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		return (shell->last_exit = 1, 1);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		builtin_echo(shell, cmd);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		shell->last_exit = builtin_cd(shell, cmd);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		shell->last_exit = builtin_pwd(shell);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		builtin_export(shell, cmd);
	else
		builtin_unset(shell, cmd);
	restore_stdio(saved_in, saved_out);
	if (ft_strcmp(cmd->args[0], "cd") != 0)
		shell->last_exit = 0;
	return (1);
}

static void	execute_external(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	environ = shell->env;
	pid = fork();
	if (pid == -1)
		return (perror("fork"), (void)(shell->last_exit = 1));
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (apply_redirections(cmd) == -1)
			exit(1);
		execvp(cmd->args[0], cmd->args);
		executor_exit_exec_error(cmd->args[0]);
	}
	waitpid(pid, &status, 0);
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
		return ((void)(shell->last_exit = 0));
	if (cmd->next)
		return (execute_pipeline(cmd, shell));
	if (execute_builtin(cmd, shell))
		return ;
	execute_external(cmd, shell);
}

static void	free_redirections(t_redir *redirs)
{
	t_redir	*next;

	while (redirs)
	{
		next = redirs->next;
		free(redirs->target);
		free(redirs);
		redirs = next;
	}
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->args && cmd->args[i])
		free(cmd->args[i++]);
	free_redirections(cmd->redirs);
	free(cmd->args);
	free(cmd->quoted);
	free(cmd->infile);
	free(cmd->outfile);
	free(cmd->heredoc_delim);
	free(cmd);
}
