#include "minishell.h"

static t_redir_type	get_redir_type(char *op)
{
	if (ft_strcmp(op, "<") == 0)
		return (R_IN);
	if (ft_strcmp(op, "<<") == 0)
		return (R_HEREDOC);
	if (ft_strcmp(op, ">>") == 0)
		return (R_APPEND);
	return (R_OUT);
}

static void	append_redir(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*current;

	if (!cmd->redirs)
	{
		cmd->redirs = new_redir;
		return ;
	}
	current = cmd->redirs;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

static int	sync_legacy_redirection(t_cmd *cmd, t_redir_type type, char *target)
{
	char	*copy;

	copy = ft_strdup(target);
	if (!copy)
		return (-1);
	if (type == R_IN)
	{
		cmd->heredoc = 0;
		free(cmd->heredoc_delim);
		cmd->heredoc_delim = NULL;
		free(cmd->infile);
		cmd->infile = copy;
	}
	else if (type == R_HEREDOC)
	{
		cmd->heredoc = 1;
		free(cmd->heredoc_delim);
		cmd->heredoc_delim = copy;
		free(cmd->infile);
		cmd->infile = NULL;
	}
	else
	{
		free(cmd->outfile);
		cmd->outfile = copy;
		cmd->append = (type == R_APPEND);
	}
	return (0);
}

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
	char			*file;
	int			was_quoted;
	t_redir			*new_redir;
	t_redir_type	type;

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
	type = get_redir_type(op);
	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
	{
		free(file);
		return (-1);
	}
	new_redir->type = type;
	new_redir->target = file;
	new_redir->next = NULL;
	if (sync_legacy_redirection(cmd, type, file) == -1)
	{
		free(file);
		free(new_redir);
		return (-1);
	}
	append_redir(cmd, new_redir);
	return (0);
}
