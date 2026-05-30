/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_count_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:05 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:28:06 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_redir_char(char c)
{
	return (c == '>' || c == '<');
}

static int	skip_quoted(char *s, int i)
{
	char	quote;

	quote = s[i++];
	while (s[i] && s[i] != quote)
		i++;
	if (!s[i])
		return (-1);
	return (i + 1);
}

static int	skip_word_token(char *s, int i)
{
	while (s[i] && !parser_is_space(s[i]) && !is_redir_char(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
			i = skip_quoted(s, i);
		else
			i++;
		if (i < 0)
			return (-1);
	}
	return (i);
}

int	parser_count_args(char *s)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		parser_skip_spaces(s, &i);
		if (!s[i])
			break ;
		count++;
		if (is_redir_char(s[i]))
			i += (s[i + 1] == s[i]) + 1;
		else
			i = skip_word_token(s, i);
		if (i < 0)
			return (-1);
	}
	return (count);
}
