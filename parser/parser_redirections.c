#include "minishell.h"

int	parser_is_redirection(char *word)
{
	if (!word)
		return (0);
	if (ft_strcmp(word, ">") == 0)
		return (1);
	if (ft_strcmp(word, ">>") == 0)
		return (1);
	if (ft_strcmp(word, "<") == 0)
		return (1);
	if (ft_strcmp(word, "<<") == 0)
		return (1);
	return (0);
}

int	parser_set_redirection(t_cmd *cmd, char *op, char *input, int *i)
{
	char	*file;
	int		was_quoted;

	parser_skip_spaces(input, i);
	if (!input[*i])
	{
		parser_set_status(2);
		ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
		return (-1);
	}
	file = parser_extract_word(input, i, &was_quoted);
	if (!file)
		return (-1);
	if (ft_strcmp(op, "<") == 0)
	{
		free(cmd->infile);
		cmd->infile = file;
	}else if(ft_strcmp(op, "<<") == 0)
	{
		if (cmd->heredoc_delim)
			free(cmd->heredoc_delim);
		cmd->heredoc_delim = file;
		cmd->heredoc = 1;
		free(cmd->infile);
		cmd->infile = NULL;
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
