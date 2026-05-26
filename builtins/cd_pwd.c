#include "minishell.h"

static int	set_shell_env_var(t_shell *shell, char *key, char *value)
{
	char	*tmp;
	char	*entry;

	if (!shell || !key || !value)
		return (1);
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (1);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!entry)
		return (1);
	export_var(shell, entry);
	free(entry);
	return (0);
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

int	builtin_pwd(t_shell *shell)
{
	char	*cwd;

	(void)shell;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return (1);
	}
	ft_printf("%s\n", cwd);
	free(cwd);
	return (0);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*target;
	char	*oldpwd;
	char	*newpwd;

	if (!shell || !cmd || !cmd->args)
		return (1);
	if (cmd->argc > 2)
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		return (1);
	}
	target = cmd->args[1];
	if (!target)
	{
		target = get_env_value(shell, "HOME");
		if (!target || target[0] == '\0')
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (1);
		}
	}
	oldpwd = getcwd(NULL, 0);
	if (chdir(target) == -1)
	{
		perror("cd");
		free(oldpwd);
		return (1);
	}
	newpwd = getcwd(NULL, 0);
	if (oldpwd)
	{
		set_shell_env_var(shell, "OLDPWD", oldpwd);
		free(oldpwd);
	}
	if (newpwd)
	{
		set_shell_env_var(shell, "PWD", newpwd);
		free(newpwd);
	}
	return (0);
}
