/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:22 by akaung            #+#    #+#             */
/*   Updated: 2026/06/02 18:02:37 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static void	set_input_redir(t_cmd *cmd, t_redir_type type, char *copy)
{
	if (type == R_HEREDOC)
	{
		cmd->heredoc = 1;
		free(cmd->heredoc_delim);
		cmd->heredoc_delim = copy;
		free(cmd->infile);
		cmd->infile = NULL;
	}
	else
	{
		cmd->heredoc = 0;
		free(cmd->heredoc_delim);
		cmd->heredoc_delim = NULL;
		free(cmd->infile);
		cmd->infile = copy;
	}
}

static int	sync_legacy_redirection(t_cmd *cmd, t_redir_type type, char *target)
{
	char	*copy;

	copy = ft_strdup(target);
	if (!copy)
		return (-1);
	if (type == R_IN || type == R_HEREDOC)
		set_input_redir(cmd, type, copy);
	else
	{
		free(cmd->outfile);
		cmd->outfile = copy;
		cmd->append = (type == R_APPEND);
	}
	return (0);
}

int	parser_set_redirection(t_cmd *cmd, char *input, int *i, t_shell *shell)
{
	char			*file;
	char			*op;
	int				was_quoted;
	t_redir_type	type;

	op = parser_extract_word(input, i, NULL);
	if (!op)
		return (-1);
	parser_skip_spaces(input, i);
	if (!input[*i])
		return (parser_redirection_error(shell, op));
	file = parser_extract_word(input, i, &was_quoted);
	if (!file)
		return (free(op), -1);
	type = get_redir_type(op);
	free(op);
	return (parser_add_redirection(cmd, file, type));
}
