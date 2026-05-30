/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 10:00:00 by aungk             #+#    #+#             */
/*   Updated: 2026/05/30 18:01:20 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parser_set_status(t_shell *shell, int status)
{
	shell->parser_status = status;
}

int	parser_get_status(t_shell *shell)
{
	return (shell->parser_status);
}

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
