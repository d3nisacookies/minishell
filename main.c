#define _POSIX_C_SOURCE 200809L

#include "minishell.h"
#include <signal.h>

volatile sig_atomic_t g_signal;

static void	free_env_copy(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

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
	shell.env = copy_env(envp);
	if (!shell.env)
	{
		shell.env = malloc(sizeof(char *));
		if (!shell.env)
			return (1);
		shell.env[0] = NULL;
	}
	shell.last_exit = 0;
	shell.should_exit = 0;
	setup_signals();
	prompt_loop(&shell);
	status = shell.last_exit;
	clear_history();
	free_env_copy(shell.env);
	return (status);
}
