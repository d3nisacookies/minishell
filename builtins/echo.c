/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 14:14:38 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 14:26:52 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_n_option(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 1;
	while (arg[i] == 'n')
		i++;
	return (arg[i] == '\0');
}

static void	print_echo_args(t_cmd *cmd, int start)
{
	int	i;

	i = start;
	while (i < cmd->argc)
	{
		ft_printf("%s", cmd->args[i]);
		if (i + 1 < cmd->argc)
			ft_printf(" ");
		i++;
	}
}

void	builtin_echo(t_shell *shell, t_cmd *cmd)
{
	int	i;

	(void)shell;
	if (!cmd || !cmd->args)
		return ;
	i = 1;
	while (i < cmd->argc && is_n_option(cmd->args[i]))
		i++;
	print_echo_args(cmd, i);
	if (i == 1)
		ft_printf("\n");
}
