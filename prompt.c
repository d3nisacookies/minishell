/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:31:39 by akaung            #+#    #+#             */
/*   Updated: 2026/06/05 11:34:33 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_segment(char **segs, int idx, t_shell *shell)
{
	char	*segment;
	char	*prev;

	segment = trim_spaces(segs[idx]);
	if (segment[0] != '\0')
	{
		if (starts_with_pipe(segment))
			return (syntax_error_token(shell, "|"));
		if (segs[idx + 1] != NULL && ends_with_pipe(segment))
			return (syntax_error_token(shell, ";"));
		return (0);
	}
	if (idx > 0)
	{
		prev = trim_spaces(segs[idx - 1]);
		if (ends_with_pipe(prev))
			return (syntax_error_token(shell, ";"));
	}
	if (!is_trailing_empty_segment(segs, idx))
	{
		if (idx > 0 && segs[idx + 1] != NULL)
			return (syntax_error_token(shell, ";;"));
		return (syntax_error_token(shell, ";"));
	}
	return (1);
}

static int	validate_segments(char **segments, t_shell *shell)
{
	int	idx;
	int	ret;

	idx = 0;
	while (segments[idx])
	{
		ret = check_segment(segments, idx, shell);
		if (ret == -1)
			return (-1);
		if (ret == 1)
			break ;
		idx++;
	}
	return (0);
}

static int	run_segments(char **segments, t_shell *shell)
{
	char	*segment;
	t_cmd	*cmd;
	int		idx;

	idx = 0;
	while (segments[idx])
	{
		segment = trim_spaces(segments[idx]);
		if (segment[0] == '\0')
			break ;
		cmd = parse_command(segment, shell);
		if (cmd == NULL)
			return (handle_parse_fail(shell, segments));
		execute_command(cmd, shell);
		free_cmd_list(cmd);
		if (shell->should_exit)
			break ;
		idx++;
	}
	return (0);
}

static int	execute_input_segments(char *input, t_shell *shell)
{
	char	**segments;
	int		status;

	segments = split_semicolons(input, shell);
	if (!segments)
	{
		status = parser_get_status(shell);
		if (!status)
			status = 1;
		shell->last_exit = status;
		return (-1);
	}
	if (validate_segments(segments, shell) == -1)
	{
		shell->last_exit = 1;
		free_split_array(segments);
		return (-1);
	}
	run_segments(segments, shell);
	free_split_array(segments);
	return (0);
}

void	prompt_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		input = readline("$> ");
		if (input == NULL)
		{
			shell->last_exit = 130;
			write(1, "\n", 1);
			break ;
		}
		if (strlen(input) == 0)
		{
			free(input);
			continue ;
		}
		add_history(input);
		execute_input_segments(input, shell);
		free(input);
		if (shell->should_exit)
			break ;
	}
}
