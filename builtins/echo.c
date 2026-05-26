#include "minishell.h"

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

static void	print_echo_arg(t_shell *shell, char *arg)
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
		value = get_env_value(shell, arg + 1);
		if (value)
			ft_printf("%s", value);
		return ;
	}
	ft_printf("%s", arg);
}

void	builtin_echo(t_shell *shell, t_cmd *cmd)
{
	int	i;
	int	is_n;

	is_n = 0;
	if (cmd->args[1] && ft_strcmp(cmd->args[1], "-n") == 0)
		is_n = 1;
	if (!shell || !cmd || !cmd->args)
		return ;
	i = 1;
	while (i < cmd->argc)
	{
		print_echo_arg(shell, cmd->args[i]);
		if (i + 1 < cmd->argc)
			ft_printf(" ");
		i++;
	}
	if (is_n == 0)
		ft_printf("\n");
}
