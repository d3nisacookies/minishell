#include "minishell.h"

static int	get_exit_sign(char *s, int *i, int *neg, unsigned long long *lim)
{
	*i = 0;
	*neg = 0;
	*lim = (unsigned long long)LLONG_MAX;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
		{
			*neg = 1;
			*lim = (unsigned long long)LLONG_MAX + 1;
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

void	builtin_exit(t_shell *shell, t_cmd *cmd)
{
	unsigned char	code;

	if (cmd->argc == 1)
	{
		shell->should_exit = 1;
		return ;
	}
	if (!parse_exit_code(cmd->args[1], &code))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		shell->last_exit = 2;
		shell->should_exit = 1;
		return ;
	}
	if (cmd->argc > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		shell->last_exit = 1;
		return ;
	}
	shell->last_exit = code;
	shell->should_exit = 1;
}
