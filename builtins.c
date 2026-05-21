#include "minishell.h"

static void	print_export(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		ft_printf("declare -x %s\n", env[i]);
		i++;
	}
}

void	builtin_export(t_shell *shell, t_cmd *cmd)
{
	int i;

	if (!shell || !cmd || !cmd->args)
		return ;
	if (cmd->argc == 1)
	{
		print_export(shell->env);
		return ;
	}
	i = 1;
	while (i < cmd->argc)
	{
		export_var(shell, cmd->args[i]);
		i++;
	}
}

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

static void	print_word_split(char *str)
{
	int	i;
	int	in_space;

	if (!str)
		return ;
	i = 0;
	in_space = 1;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
		{
			in_space = 1;
			i++;
		}
		else
		{
			if (in_space && i > 0)
				ft_printf(" ");
			ft_printf("%c", str[i]);
			in_space = 0;
			i++;
		}
	}
}

static void	print_echo_arg(t_shell *shell, char *arg, int was_quoted)
{
	char	*value;

	if (!arg)
		return ;
	if (arg[0] == '$' && arg[1] != '\0')
	{
		if (arg[1] == '?' && arg[2] == '\0')
		{
			ft_printf("%d", shell->last_exit);
			return ;
		}
		if (is_only_variable(arg))
		{
			value = get_env_value(shell, arg + 1);
			if (value)
			{
				if (was_quoted)
					ft_printf("%s", value);
				else
					print_word_split(value);
			}
			return ;
		}
		value = get_env_value(shell, arg + 1);
		if (value)
		{
			ft_printf("%s", value);
			return ;
		}
	}
	ft_printf("%s", arg);
}

static int	is_echo_n_option(char *arg)
{
	int	j;

	if (!arg || arg[0] != '-' || arg[1] == '\0')
		return (0);
	j = 1;
	while (arg[j])
	{
		if (arg[j] != 'n')
			return (0);
		j++;
	}
	return (1);
}

void	builtin_echo(t_shell *shell, t_cmd *cmd)
{
	int	i;
	int	n_flag;

	if (!shell || !cmd || !cmd->args)
		return ;
	i = 1;
	n_flag = 0;
	while (i < cmd->argc && is_echo_n_option(cmd->args[i]))
	{
		n_flag = 1;
		i++;
	}
	while (i < cmd->argc)
	{
		print_echo_arg(shell, cmd->args[i], cmd->quoted[i]);
		if (i + 1 < cmd->argc)
			ft_printf(" ");
		i++;
	}
	if (!n_flag)
		ft_printf("\n");
}

int is_only_variable(char *str)
{
    int i;

    if (!str || str[0] != '$' || str[1] == '\0')
        return (0);
    i = 1;
    if (str[1] == '?' && str[2] == '\0')
        return (1);
    while (str[i])
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

static void	free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

static char	*normalize_absolute_path(char *path)
{
	char	**parts;
	char	**stack;
	char	*result;
	size_t	len;
	int		i;
	int		top;
	int		j;

	if (!path || path[0] != '/')
		return (NULL);
	parts = ft_split(path, '/');
	if (!parts)
		return (NULL);
	stack = malloc(sizeof(char *) * (ft_strlen(path) + 1));
	if (!stack)
		return (free_split(parts), NULL);
	top = 0;
	i = 0;
	while (parts[i])
	{
		if (ft_strcmp(parts[i], ".") == 0 || parts[i][0] == '\0')
			free(parts[i]);
		else if (ft_strcmp(parts[i], "..") == 0)
		{
			free(parts[i]);
			if (top > 0)
				free(stack[--top]);
		}
		else
			stack[top++] = parts[i];
		i++;
	}
	len = 2;
	for (j = 0; j < top; j++)
		len += ft_strlen(stack[j]) + 1;
	result = malloc(len);
	if (!result)
		return (free(stack), free_split(parts), NULL);
	result[0] = '/';
	result[1] = '\0';
	j = 0;
	while (j < top)
	{
		ft_strlcat(result, stack[j], len);
		if (j + 1 < top)
			ft_strlcat(result, "/", len);
		j++;
	}
	j = 0;
	while (j < top)
		free(stack[j++]);
	free(stack);
	free(parts);
	return (result);
}

static char	*join_paths(char *base, char *child)
{
	char	*tmp;
	char	*joined;

	if (!base || !child)
		return (NULL);
	if (ft_strcmp(base, "/") == 0)
		return (ft_strjoin("/", child));
	tmp = ft_strjoin(base, "/");
	if (!tmp)
		return (NULL);
	joined = ft_strjoin(tmp, child);
	free(tmp);
	return (joined);
}

static void	set_env_value(t_shell *shell, char *key, char *value)
{
	char	*tmp;
	char	*entry;

	if (!shell || !key || !value)
		return ;
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return ;
	entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!entry)
		return ;
	export_var(shell, entry);
	free(entry);
}

static char	*compute_logical_pwd(char *current_pwd, char *target)
{
	char	*joined;
	char	*normalized;

	if (!target || target[0] == '\0')
		return (NULL);
	if (target[0] == '/')
		return (normalize_absolute_path(target));
	if (!current_pwd || current_pwd[0] != '/')
		return (NULL);
	joined = join_paths(current_pwd, target);
	if (!joined)
		return (NULL);
	normalized = normalize_absolute_path(joined);
	free(joined);
	return (normalized);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*target;
	char	*old_pwd;
	char	*fallback_old;
	char	*new_pwd;
	char	*fallback_new;

	if (!shell || !cmd || !cmd->args)
		return (1);
	if (cmd->argc > 2)
		return (ft_printf("cd: too many arguments\n"), 1);
	target = cmd->argc == 1 ? get_env_value(shell, "HOME") : cmd->args[1];
	if (!target || target[0] == '\0')
		return (ft_printf("cd: HOME not set\n"), 1);
	old_pwd = get_env_value(shell, "PWD");
	fallback_old = NULL;
	if (!old_pwd || old_pwd[0] == '\0')
		fallback_old = getcwd(NULL, 0);
	new_pwd = compute_logical_pwd(old_pwd, target);
	if (chdir(target) == -1)
		return (free(new_pwd), free(fallback_old), perror("cd"), 1);
	if (old_pwd && old_pwd[0] != '\0')
		set_env_value(shell, "OLDPWD", old_pwd);
	else if (fallback_old)
		set_env_value(shell, "OLDPWD", fallback_old);
	fallback_new = getcwd(NULL, 0);
	if (new_pwd)
		set_env_value(shell, "PWD", new_pwd);
	else if (fallback_new)
		set_env_value(shell, "PWD", fallback_new);
	free(new_pwd);
	free(fallback_old);
	free(fallback_new);
	return (0);
}

int	builtin_pwd(t_shell *shell)
{
	char	*pwd;
	char	*fallback;

	pwd = get_env_value(shell, "PWD");
	if (pwd && pwd[0] != '\0')
	{
		ft_printf("%s\n", pwd);
		return (0);
	}
	fallback = getcwd(NULL, 0);
	if (!fallback)
		return (perror("pwd"), 1);
	ft_printf("%s\n", fallback);
	free(fallback);
	return (0);
}