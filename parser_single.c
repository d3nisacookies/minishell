#include "minishell.h"

static void	free_partial_cmd(t_cmd *cmd, int count)
{
	while (count > 0)
		free(cmd->args[--count]);
	free(cmd->args);
	free(cmd->quoted);
	free(cmd);
}

static int	fill_args(t_cmd *cmd, char *input)
{
	int	i;
	int	count;
	int	was_quoted;

	i = 0;
	count = 0;
	while (input[i])
	{
		parser_skip_spaces(input, &i);
		if (!input[i])
			break ;
		cmd->args[count] = parser_extract_word(input, &i, &was_quoted);
		if (!cmd->args[count])
			return (-1);
		cmd->quoted[count] = was_quoted;
		count++;
	}
	cmd->args[count] = NULL;
	cmd->argc = count;
	cmd->next = NULL;
	return (0);
}

static t_cmd	*alloc_cmd(int argc)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = malloc(sizeof(char *) * (argc + 1));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->quoted = malloc(sizeof(int) * (argc + 1));
	if (!cmd->quoted)
	{
		free(cmd->args);
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

t_cmd	*parse_single(char *input)
{
	t_cmd	*cmd;
	int		argc;

	if (!input || !*input)
		return (NULL);
	argc = parser_count_args(input);
	if (argc < 0)
	{
		parser_put_unmatched_quote_error();
		return (NULL);
	}
	cmd = alloc_cmd(argc);
	if (!cmd)
		return (NULL);
	if (fill_args(cmd, input) == -1)
	{
		free_partial_cmd(cmd, argc);
		parser_put_unmatched_quote_error();
		return (NULL);
	}
	return (cmd);
}
