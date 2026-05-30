#include "minishell.h"

typedef struct s_expand_build
{
	char	**new_args;
	int		*new_quoted;
	int		i;
	int		count;
} 				t_expand_build;

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

static int	expand_dollar(t_shell *shell, char *arg, int *i, char **result)
{
	char	*chunk;
	char	*key;
	int		start;

	if (arg[*i + 1] == '?')
	{
		chunk = ft_itoa(shell->last_exit);
		*result = append_str(*result, chunk);
		free(chunk);
		*i += 2;
		return (*result != NULL);
	}
	if (!is_var_start(arg[*i + 1]))
		return (*result = append_str(*result, "$"), (*i)++, *result != NULL);
	start = ++(*i);
	while (arg[*i] && is_var_char(arg[*i]))
		(*i)++;
	key = ft_substr(arg, start, *i - start);
	*result = append_str(*result, get_env_value(shell, key));
	free(key);
	return (*result != NULL);
}

static char	*expand_argument(t_shell *shell, char *arg, int quote)
{
	char	*result;
	char	chr[2];
	int		i;

	if (!arg || quote == '\'')
		return (ft_strdup(arg));
	result = ft_strdup("");
	i = 0;
	while (result && arg[i])
	{
		if (arg[i] != '$')
		{
			chr[0] = arg[i++];
			chr[1] = '\0';
			result = append_str(result, chr);
		}
		else if (!expand_dollar(shell, arg, &i, &result))
			break ;
	}
	return (result);
}

static int	fill_expanded_args(t_cmd *cmd, t_shell *shell, t_expand_build *b)
{
	char	*expanded;

	while (b->i < cmd->argc)
	{
		expanded = expand_argument(shell, cmd->args[b->i], cmd->quoted[b->i]);
		if (!expanded)
			return (-1);
		free(cmd->args[b->i]);
		if (expanded[0] || cmd->quoted[b->i])
		{
			b->new_args[b->count] = expanded;
			b->new_quoted[b->count++] = cmd->quoted[b->i];
		}
		else
			free(expanded);
		b->i++;
	}
	return (0);
}

int	executor_expand_args(t_cmd *cmd, t_shell *shell)
{
	t_expand_build	b;

	b.new_args = malloc(sizeof(char *) * (cmd->argc + 1));
	b.new_quoted = malloc(sizeof(int) * (cmd->argc + 1));
	if (!b.new_args || !b.new_quoted)
		return (free(b.new_args), free(b.new_quoted), -1);
	b.i = 0;
	b.count = 0;
	if (fill_expanded_args(cmd, shell, &b) == -1)
		return (free(b.new_args), free(b.new_quoted), -1);
	b.new_args[b.count] = NULL;
	free(cmd->args);
	free(cmd->quoted);
	cmd->args = b.new_args;
	cmd->quoted = b.new_quoted;
	cmd->argc = b.count;
	return (0);
}
