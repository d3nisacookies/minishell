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
#include <signal.h>

volatile sig_atomic_t	g_signal;

static void	signal_handler(int signum)
{
	g_signal = signum;
	if (signum == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = signal_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	rl_catch_signals = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

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
	setup_signals();
	prompt_loop(&shell);
	status = shell.last_exit;
	rl_clear_history();
	free_shell_state(&shell);
	return (status);
}
