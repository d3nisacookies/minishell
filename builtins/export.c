/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 14:27:21 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 14:27:22 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_identifier(char *arg)
{
	int	i;

	if (!arg || arg[0] == '\0')
		return (0);
	if (!(ft_isalpha(arg[0]) || arg[0] == '_'))
		return (0);
	i = 1;
	while (arg[i] && arg[i] != '=')
	{
		if (!(ft_isalnum(arg[i]) || arg[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static void	print_export_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

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
	shell->last_exit = 0;
	if (cmd->argc == 1)
	{
		print_export(shell->exported);
		return ;
	}
	i = 1;
	while (i < cmd->argc)
	{
		if (!is_valid_identifier(cmd->args[i]))
		{
			print_export_error(cmd->args[i]);
			shell->last_exit = 1;
		}
		else
			export_var(shell, cmd->args[i]);
		i++;
	}
}
