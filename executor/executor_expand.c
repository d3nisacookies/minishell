#include "minishell.h"

static char	*lookup_env_value(t_shell *shell, char *key)
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

static char	*append_str(char *dst, char *src)
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

static char	*expand_argument(t_shell *shell, char *arg, int quote)
{
	char	*result;
	char	*chunk;
	char	*key;
	int		i;
	int		start;

	if (!arg || quote == '\'')
		return (ft_strdup(arg));
	result = ft_strdup("");
	i = 0;
	while (result && arg[i])
	{
		if (arg[i] != '$')
			result = append_str(result, (char[2]){arg[i++], '\0'});
		else if (arg[i + 1] == '?')
		{
			chunk = ft_itoa(shell->last_exit);
			result = append_str(result, chunk);
			free(chunk);
			i += 2;
		}
		else if (!ft_isalpha(arg[i + 1]) && arg[i + 1] != '_')
			result = append_str(result, (char[2]){arg[i++], '\0'});
		else
		{
			start = ++i;
			while (arg[i] && (ft_isalnum(arg[i]) || arg[i] == '_'))
				i++;
			key = ft_substr(arg, start, i - start);
			result = append_str(result, lookup_env_value(shell, key));
			free(key);
		}
	}
	return (result);
}

static int	rebuild_args(t_cmd *cmd, char **new_args, int *new_quoted,
		int count)
{
	new_args[count] = NULL;
	free(cmd->args);
	free(cmd->quoted);
	cmd->args = new_args;
	cmd->quoted = new_quoted;
	cmd->argc = count;
	return (0);
}

int	executor_expand_args(t_cmd *cmd, t_shell *shell)
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
		if (expanded[0] || cmd->quoted[i])
		{
			new_args[count] = expanded;
			new_quoted[count++] = cmd->quoted[i];
		}
		else
			free(expanded);
		i++;
	}
	return (rebuild_args(cmd, new_args, new_quoted, count));
}
