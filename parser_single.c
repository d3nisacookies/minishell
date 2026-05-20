#include "minishell.h"


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
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->next = NULL;
	return (cmd);
}

static int	is_redirection(char *word)
{
	if (ft_strcmp(word, ">") == 0)
		return (1);
	if (ft_strcmp(word, ">>") == 0)
		return (1);
	if (ft_strcmp(word, "<") == 0)
		return (1);
	return (0);
}

static int	set_redirection(t_cmd *cmd, char *op, char *input, int *i)
{
	char	*file;

	parser_skip_spaces(input, i);
	if (!input[*i])
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
		return (-1);
	}
	file = parser_extract_word(input, i);
	if (!file)
		return (-1);
	if (ft_strcmp(op, "<") == 0)
	{
		free(cmd->infile);
		cmd->infile = file;
	}
	else
	{
		free(cmd->outfile);
		cmd->outfile = file;
		if (ft_strcmp(op, ">>") == 0)
			cmd->append = 1;
		else
			cmd->append = 0;
	}
	return (0);
}

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

static int	fill_args(t_cmd *cmd, char *input)
{
	int		i;
	int		count;
	char	*word;

	i = 0;
	count = 0;
	while (input[i])
	{
		parser_skip_spaces(input, &i);
		if (!input[i])
			break ;
		word = parser_extract_word(input, &i);
		if (!word)
			return (-1);
		if (is_redirection(word))
		{
			if (set_redirection(cmd, word, input, &i) == -1)
			{
				free(word);
				return (-1);
			}
			free(word);
		}
		else
		{
			cmd->args[count] = word;
			count++;
		}
	}
	cmd->args[count] = NULL;
	cmd->argc = count;
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
