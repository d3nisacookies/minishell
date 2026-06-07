/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_util.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 02:34:00 by Copilot           #+#    #+#             */
/*   Updated: 2026/06/08 02:34:00 by Copilot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	heredoc_sigint(int signum)
{
	(void)signum;
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_done = 1;
}

static int	setup_heredoc_signals(struct sigaction *old_int,
		struct sigaction *old_quit)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = heredoc_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	g_signal = 0;
	if (sigaction(SIGINT, &sa_int, old_int) == -1)
		return (perror("sigaction"), -1);
	if (sigaction(SIGQUIT, &sa_quit, old_quit) == -1)
	{
		sigaction(SIGINT, old_int, NULL);
		return (perror("sigaction"), -1);
	}
	return (0);
}

static int	write_heredoc_line(int fd, char *line, t_shell *shell, int quoted)
{
	char	*expanded;

	if (quoted)
		expanded = ft_strdup(line);
	else
		expanded = expand_argument(shell, line, 0);
	if (!expanded)
		return (-1);
	ft_putstr_fd(expanded, fd);
	ft_putchar_fd('\n', fd);
	free(expanded);
	return (0);
}

static int	fill_heredoc_pipe(int *pipefd, char *delimiter, t_shell *shell,
		int quoted)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal == SIGINT)
			return (free(line), -1);
		if (!line)
			break ;
		if (ft_strcmp(line, delimiter) == 0)
			return (free(line), 0);
		if (write_heredoc_line(pipefd[1], line, shell, quoted) == -1)
			return (free(line), -1);
		free(line);
	}
	return (0);
}

int	redirect_heredoc(char *delimiter, t_shell *shell, int quoted)
{
	struct sigaction	old_int;
	struct sigaction	old_quit;
	int					pipefd[2];

	if (pipe(pipefd) == -1)
		return (perror("pipe"), -1);
	if (setup_heredoc_signals(&old_int, &old_quit) == -1)
		return (close(pipefd[0]), close(pipefd[1]), -1);
	if (fill_heredoc_pipe(pipefd, delimiter, shell, quoted) == -1)
	{
		sigaction(SIGINT, &old_int, NULL);
		sigaction(SIGQUIT, &old_quit, NULL);
		close(pipefd[0]);
		close(pipefd[1]);
		if (g_signal == SIGINT)
			shell->last_exit = 130;
		return (-1);
	}
	sigaction(SIGINT, &old_int, NULL);
	sigaction(SIGQUIT, &old_quit, NULL);
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		return (close(pipefd[0]), perror("dup2"), -1);
	close(pipefd[0]);
	return (0);
}
