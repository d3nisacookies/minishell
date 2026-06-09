/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_single.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:33 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:03:02 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_partial_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	if (cmd->args)
		cmd->args[cmd->argc] = NULL;
	free_cmd(cmd);
}

static int	handle_word(t_cmd *cmd, char *input, int *i, t_shell *shell)
{
	int		was_quoted;
	char	*word;

	if (input[*i] == '<' || input[*i] == '>')
		return (parser_set_redirection(cmd, input, i, shell));
	word = parser_extract_word(input, i, &was_quoted);
	if (!word)
		return (-1);
	if (!parser_is_redirection(word))
	{
		cmd->args[cmd->argc] = word;
		cmd->quoted[cmd->argc] = was_quoted;
		cmd->argc++;
		return (0);
	}
	return (free(word), -1);
}

static int	fill_args(t_cmd *cmd, char *input, t_shell *shell)
{
	int	i;

	i = 0;
	while (input[i])
	{
		parser_skip_spaces(input, &i);
		if (!input[i])
			break ;
		if (handle_word(cmd, input, &i, shell) == -1)
			return (-1);
	}
	cmd->args[cmd->argc] = NULL;
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
		return (free(cmd), NULL);
	cmd->quoted = malloc(sizeof(int) * (argc + 1));
	if (!cmd->quoted)
		return (free(cmd->args), free(cmd), NULL);
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->next = NULL;
	cmd->heredoc = 0;
	cmd->redirs = NULL;
	cmd->heredoc_delim = NULL;
	cmd->argc = 0;
	return (cmd);
}

t_cmd	*parse_single(char *input, t_shell *shell)
{
	t_cmd	*cmd;
	int		argc;

	if (!input || !*input)
		return (NULL);
	argc = parser_count_args(input);
	if (argc < 0)
	{
		parser_put_quote_error(shell);
		return (NULL);
	}
	cmd = alloc_cmd(argc);
	if (!cmd)
		return (NULL);
	if (fill_args(cmd, input, shell) == -1)
	{
		free_partial_cmd(cmd);
		parser_put_quote_error(shell);
		return (NULL);
	}
	return (cmd);
}
