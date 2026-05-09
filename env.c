#include "libft/libft.h"
#include "minishell.h"

static int	count_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	return (i);
}

char	**copy_env(char **envp)
{
	char	**env;
	int		len;
	int		i;

	len = count_env(envp);
	env = malloc(sizeof(char *) * (len + 1));
	if (!env)
		return (NULL);
	i = 0;
	while (i < len)
	{
		env[i] = ft_strdup(envp[i]);
		if (!env[i])
			return (NULL);
		i++;
	}
	env[i] = NULL;
	return (env);
}

int	find_env_index(char **env, char *key)
{
	int	i;
	int	key_len;

	key_len = ft_strlen(key);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0 && (env[i][key_len] == '='
				|| env[i][key_len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

char	*make_env_entry(char *key, char *value)
{
	char	*tmp;
	char	*entry;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

void	append_env(char ***env, char *new_entry)
{
	char	**new_env;
	int		len;
	int		i;

	len = count_env(*env);
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return ;
	i = 0;
	while (i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(*env);
	*env = new_env;
}

void	export_var(t_shell *shell, char *arg)
{
	char	*equals;
	char	*key;
	char	*value;
	int		idx;
	char	*new_entry;

	equals = ft_strchr(arg, '=');
	if (!equals)
	{
		if (find_env_index(shell->env, arg) == -1)
			append_env(&shell->env, ft_strdup(arg));
		return ;
	}
	key = ft_substr(arg, 0, equals - arg);
	if (!key)
		return ;
	value = equals + 1;
	new_entry = make_env_entry(key, value);
	free(key);
	if (!new_entry)
		return ;
	idx = find_env_index(shell->env, key);
	if (idx != -1)
	{
		free(shell->env[idx]);
		shell->env[idx] = new_entry;
	}
	else
	{
		append_env(&shell->env, new_entry);
	}
}
