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
	int	i;

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
