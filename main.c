/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 18:10:30 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 18:10:43 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _POSIX_C_SOURCE 200809L

#include "minishell.h"
volatile sig_atomic_t	g_signal;

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;
	int		status;

	(void)ac;
	(void)av;
	if (init_shell_state(&shell, envp))
		return (1);
	shell.last_exit = 0;
	shell.should_exit = 0;
	init_shlvl(&shell);
	if (setup_shell_signals() == -1)
		return (free_shell_state(&shell), 1);
	prompt_loop(&shell);
	status = shell.last_exit;
	rl_clear_history();
	free_shell_state(&shell);
	return (status);
}
