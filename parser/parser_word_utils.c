/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:46 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:28:47 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parser_is_redir_char(char c)
{
	return (c == '>' || c == '<');
}

int	parser_skip_quoted(char *s, int i)
{
	char	quote;

	quote = s[i++];
	while (s[i] && s[i] != quote)
		i++;
	if (!s[i])
		return (-1);
	return (i + 1);
}

int	parser_get_redir_len(char *s, int i)
{
	if (!parser_is_redir_char(s[i]))
		return (0);
	if (s[i + 1] == s[i])
		return (2);
	return (1);
}
