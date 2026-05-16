/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aungk <aungk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 10:00:00 by aungk             #+#    #+#             */
/*   Updated: 2026/05/16 10:00:00 by aungk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parser_is_space(char c)
{
	return (c == ' ' || c == '\t');
}

void	parser_skip_spaces(char *s, int *i)
{
	while (s[*i] && parser_is_space(s[*i]))
		(*i)++;
}

void	parser_free_split(char **split, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(split[i]);
		i++;
	}
	free(split);
}

void	parser_put_unmatched_quote_error(void)
{
	ft_putstr_fd("minishell: syntax error: unmatched quote\n", 2);
}

void	parser_put_pipe_error(void)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
}
