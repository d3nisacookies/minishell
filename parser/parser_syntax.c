/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:41 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:03:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parser_validate_syntax(char *input, t_shell *shell)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (input[i])
	{
		if (!quote && (input[i] == '\'' || input[i] == '"'))
			quote = input[i];
		else if (quote && input[i] == quote)
			quote = 0;
		i++;
	}
	if (quote)
		return (parser_put_quote_error(shell), -1);
	return (0);
}
