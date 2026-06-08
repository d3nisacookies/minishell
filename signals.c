/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:55:57 by Copilot           #+#    #+#             */
/*   Updated: 2026/06/08 18:55:57 by Copilot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	setup_shell_signals(void)
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
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (perror("sigaction"), -1);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (perror("sigaction"), -1);
	return (0);
}

int	ignore_shell_signals(struct sigaction *old_int, struct sigaction *old_quit)
{
	struct sigaction	sa_ign;

	sa_ign.sa_handler = SIG_IGN;
	sigemptyset(&sa_ign.sa_mask);
	sa_ign.sa_flags = 0;
	if (sigaction(SIGINT, &sa_ign, old_int) == -1)
		return (perror("sigaction"), -1);
	if (sigaction(SIGQUIT, &sa_ign, old_quit) == -1)
	{
		sigaction(SIGINT, old_int, NULL);
		return (perror("sigaction"), -1);
	}
	return (0);
}

int	restore_shell_signals(struct sigaction *old_int, struct sigaction *old_quit)
{
	if (sigaction(SIGINT, old_int, NULL) == -1)
		return (perror("sigaction"), -1);
	if (sigaction(SIGQUIT, old_quit, NULL) == -1)
		return (perror("sigaction"), -1);
	return (0);
}
