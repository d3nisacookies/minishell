/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_semicolon_count.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:26 by akaung            #+#    #+#             */
/*   Updated: 2026/06/02 18:19:15 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parser_count_semicolons(char *input)
{
	int		i;
	int		count;
	char	quote;

	i = 0;
	count = 0;
	quote = 0;
	while (input[i])
	{
		if (quote == 0 && (input[i] == '\'' || input[i] == '"'))
			quote = input[i];
		else if (quote != 0 && input[i] == quote)
			quote = 0;
		else if (quote == 0 && input[i] == ';')
			count++;
		i++;
	}
	return (count);
}

int	parser_redirection_error(t_shell *shell, char *op)
{
	parser_set_status(shell, 2);
	ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n",
		2);
	free(op);
	return (-1);
}

t_redir_type	get_redir_type(char *op)
{
	if (ft_strcmp(op, "<") == 0)
		return (R_IN);
	if (ft_strcmp(op, "<<") == 0)
		return (R_HEREDOC);
	if (ft_strcmp(op, ">>") == 0)
		return (R_APPEND);
	return (R_OUT);
}
