/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** command executor
*/

#include "minishell.h"
#include <sys/stat.h>

extern char **environ;

static char	*get_env_value(t_shell *shell, char *key)
{
	int		idx;
	char	*equals;

	if (!shell || !shell->env || !key || key[0] == '\0')
		return (NULL);
	idx = find_env_index(shell->env, key);
	if (idx == -1)
		return (NULL);
	equals = ft_strchr(shell->env[idx], '=');
	if (!equals)
		return ("");
	return (equals + 1);
}

static char	*append_text(char *dst, char *src)
{
	char	*tmp;

	if (!src)
		return (dst);
	if (!dst)
		return (ft_strdup(src));
	tmp = ft_strjoin(dst, src);
	free(dst);
	return (tmp);
}

static char	*append_char(char *dst, char c)
{
	char	buffer[2];

	buffer[0] = c;
	buffer[1] = '\0';
	return (append_text(dst, buffer));
}

static int	is_var_start(char c)
{
	return (ft_isalpha(c) || c == '_');
}

static int	is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

static char	*expand_argument(t_shell *shell, char *arg, int quote)
{
	char	*expanded;
	char	*key;
	char	*value;
	int		start;
	int		i;

	if (quote == '\'' || !arg)
		return (ft_strdup(arg));
	expanded = ft_strdup("");
	if (!expanded)
		return (NULL);
	i = 0;
	while (arg[i])
	{
		if (arg[i] != '$')
			expanded = append_char(expanded, arg[i++]);
		else if (arg[i + 1] == '?')
		{
			value = ft_itoa(shell->last_exit);
			expanded = append_text(expanded, value);
			free(value);
			i += 2;
		}
		else if (!is_var_start(arg[i + 1]))
			expanded = append_char(expanded, arg[i++]);
		else
		{
			start = ++i;
			while (arg[i] && is_var_char(arg[i]))
				i++;
			key = ft_substr(arg, start, i - start);
			if (!key)
				return (free(expanded), NULL);
			value = get_env_value(shell, key);
			expanded = append_text(expanded, value);
			free(key);
		}
		if (!expanded)
			return (NULL);
	}
	return (expanded);
}

static int	expand_cmd_args(t_cmd *cmd, t_shell *shell)
{
	char	**new_args;
	int		*new_quoted;
	char	*expanded;
	int		i;
	int		count;

	new_args = malloc(sizeof(char *) * (cmd->argc + 1));
	new_quoted = malloc(sizeof(int) * (cmd->argc + 1));
	if (!new_args || !new_quoted)
		return (free(new_args), free(new_quoted), -1);
	i = 0;
	count = 0;
	while (i < cmd->argc)
	{
		expanded = expand_argument(shell, cmd->args[i], cmd->quoted[i]);
		if (!expanded)
			return (free(new_args), free(new_quoted), -1);
		free(cmd->args[i]);
		if (expanded[0] != '\0' || cmd->quoted[i])
		{
			new_args[count] = expanded;
			new_quoted[count++] = cmd->quoted[i];
		}
		else
			free(expanded);
		i++;
	}
	new_args[count] = NULL;
	free(cmd->args);
	free(cmd->quoted);
	cmd->args = new_args;
	cmd->quoted = new_quoted;
	cmd->argc = count;
	return (0);
}

static void	exit_if_directory(char *cmd_name)
{
	struct stat	st;

	if (!cmd_name)
		return ;
	if (!ft_strchr(cmd_name, '/'))
		return ;
	if (stat(cmd_name, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		exit(126);
	}
}



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

static void	print_exit_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
}

static void	builtin_exit(t_shell *shell, t_cmd *cmd)
{
	unsigned char	code;

	if (cmd->argc == 1)
		exit(shell->last_exit);
	if (!parse_exit_code(cmd->args[1], &code))
	{
		print_exit_numeric_error(cmd->args[1]);
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

static void	exit_exec_error(char *cmd_name)
{
	exit_if_directory(cmd_name);
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

void	execute_command(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->args)
		return ;
	if (expand_cmd_args(cmd, shell) == -1)
	{
		shell->last_exit = 1;
		return ;
	}
	if (!cmd->args[0])
	{
		shell->last_exit = 0;
		return ;
}
	if (cmd->next)
	{
		execute_pipeline(cmd, shell);
		return ;
	}
	if (ft_strcmp(cmd->args[0], "exit") == 0)
	{
	builtin_exit(shell, cmd);
	return ;
	}
	if (ft_strcmp(cmd->args[0], "echo") == 0)
	{
		int	saved_in;
		int	saved_out;

		if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		{
			shell->last_exit = 1;
			return ;
		}
		builtin_echo(shell, cmd);
		restore_stdio(saved_in, saved_out);
		shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "cd") == 0)
	{
	int	saved_in;
	int	saved_out;

	if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
	{
		shell->last_exit = 1;
		return ;
	}
	shell->last_exit = builtin_cd(shell, cmd);
	restore_stdio(saved_in, saved_out);
	return ;
	}
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
	{
				int	saved_in;
		int	saved_out;

		if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		{
			shell->last_exit = 1;
			return ;
		}
		builtin_pwd(shell);
		restore_stdio(saved_in, saved_out);
		shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "export") == 0)
	{
				int	saved_in;
		int	saved_out;

		if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		{
			shell->last_exit = 1;
			return ;
		}
		builtin_export(shell, cmd);
		restore_stdio(saved_in, saved_out);
		// shell->last_exit = 0;
		return ;
	}
	if (ft_strcmp(cmd->args[0], "unset") == 0)
	{
		int	saved_in;
		int	saved_out;

		if (prepare_builtin_redirs(cmd, &saved_in, &saved_out) == -1)
		{
			shell->last_exit = 1;
			return ;
		}
		builtin_unset(shell, cmd);
		restore_stdio(saved_in, saved_out);
		shell->last_exit = 0;
		return ;
	}
	environ = shell->env;
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		shell->last_exit = 1;
		return ;
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (apply_redirections(cmd) == -1)
			exit(1);
		execvp(cmd->args[0], cmd->args);
		exit_exec_error(cmd->args[0]);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			shell->last_exit = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->last_exit = 128 + WTERMSIG(status);
	}
}

void	free_cmd(t_cmd *cmd)
{
	int i;
	t_redir	*current;
	t_redir	*next;

	if (cmd == NULL)
		return ;
	i = 0;
	while (cmd->args[i] != NULL)
	{
		free(cmd->args[i]);
		i++;
	}
	current = cmd->redirs;
	while (current != NULL)
	{
		next = current->next;
		free(current->target);
		free(current);
		current = next;
	}
	free(cmd->args);
	free(cmd->quoted);
	free(cmd->infile);
	free(cmd->outfile);
	free(cmd->heredoc_delim);
	free(cmd);
}
