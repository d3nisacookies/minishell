#include "minishell.h"


volatile sig_atomic_t g_signal;

static void	signal_handler(int signum)
{
	g_signal = signum;
}

static void	setup_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

int	main(void)
{
	prompt_loop();
	return (0);
}
