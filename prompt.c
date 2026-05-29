#include "minishell.h"

static char	*trim_spaces(char *s)
{
	int	start;
	int	end;

	if (!s)
		return (NULL);
	start = 0;
	while (s[start] && parser_is_space(s[start]))
		start++;
	end = ft_strlen(s);
	while (end > start && parser_is_space(s[end - 1]))
		end--;
	s[end] = '\0';
	return (s + start);
}

static void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		free_cmd(cmd);
		cmd = next;
	}
}

static int	is_trailing_empty_segment(char **segments, int index)
{
	if (index == 0)
		return (0);
	if (segments[index + 1] != NULL)
		return (0);
	return (1);
}

static int	ends_with_pipe(char *segment)
{
	int	end;

	end = ft_strlen(segment);
	while (end > 0 && parser_is_space(segment[end - 1]))
		end--;
	if (end == 0)
		return (0);
	return (segment[end - 1] == '|');
}

static int	starts_with_pipe(char *segment)
{
	int	start;

	start = 0;
	while (segment[start] && parser_is_space(segment[start]))
		start++;
	return (segment[start] == '|');
}

static int	syntax_error_token(t_shell *shell, char *token)
{
	parser_set_status(2);
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token, 2);
	ft_putstr_fd("'\n", 2);
	shell->last_exit = 2;
	return (-1);
}

static int	validate_segments(char **segments, t_shell *shell)
{
	int		idx;
	char	*segment;
	char	*prev;

	idx = 0;
	while (segments[idx])
	{
		segment = trim_spaces(segments[idx]);
		if (segment[0] == '\0')
		{
			if (idx > 0)
			{
				prev = trim_spaces(segments[idx - 1]);
				if (ends_with_pipe(prev))
					return (syntax_error_token(shell, ";"));
			}
			if (!is_trailing_empty_segment(segments, idx))
			{
				if (idx > 0 && segments[idx + 1] != NULL)
					return (syntax_error_token(shell, ";;"));
				return (syntax_error_token(shell, ";"));
			}
			break ;
		}
		if (starts_with_pipe(segment))
			return (syntax_error_token(shell, "|"));
		if (segments[idx + 1] != NULL && ends_with_pipe(segment))
			return (syntax_error_token(shell, ";"));
		idx++;
	}
	return (0);
}

static int	execute_input_segments(char *input, t_shell *shell)
{
	char	**segments;
	char	*segment;
	t_cmd	*cmd;
	int		idx;

	segments = split_semicolons(input);
	if (!segments)
		return (shell->last_exit = parser_get_status() ? parser_get_status() : 1, -1);
	if (validate_segments(segments, shell) == -1)
	{
		shell->last_exit = 1;
		return (free_split_array(segments), -1);
	}
	idx = 0;
	while (segments[idx])
	{
		segment = trim_spaces(segments[idx]);
		if (segment[0] == '\0')
			break ;
		cmd = parse_command(segment);
		if (cmd == NULL)
		{
			shell->last_exit = parser_get_status() ? parser_get_status() : 1;
			return (free_split_array(segments), -1);
		}
		execute_command(cmd, shell);
		free_cmd_list(cmd);
		idx++;
	}
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
	}
}
