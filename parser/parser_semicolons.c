/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_semicolons.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:30 by akaung            #+#    #+#             */
/*   Updated: 2026/06/09 07:03:04 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	quote_state_changed(char *quote, char c)
{
	if (*quote == 0 && (c == '\'' || c == '"'))
		return (*quote = c, 1);
	if (*quote != 0 && c == *quote)
		return (*quote = 0, 1);
	return (0);
}

static int	append_segment(char **list, int list_index, char *segment)
{
	list[list_index] = segment;
	if (!list[list_index])
		return (-1);
	return (list_index + 1);
}

static int	fill_loop(char **list, char *input, int *start, char *quote)
{
	int	i;
	int	list_index;

	i = 0;
	list_index = 0;
	while (input[i])
	{
		if (!quote_state_changed(quote, input[i]) && *quote == 0
			&& input[i] == ';')
		{
			list_index = append_segment(list, list_index, ft_substr(input,
						*start, i - *start));
			if (list_index == -1)
				return (-1);
			*start = i + 1;
		}
		i++;
	}
	return (list_index);
}

static int	fill_segments(char **list, char *input, t_shell *shell)
{
	int		start;
	int		list_index;
	char	quote;

	start = 0;
	quote = 0;
	list_index = fill_loop(list, input, &start, &quote);
	if (list_index == -1)
		return (-1);
	if (quote != 0)
	{
		list[list_index] = NULL;
		return (parser_put_quote_error(shell), -1);
	}
	list_index = append_segment(list, list_index, ft_substr(input, start,
				ft_strlen(input) - start));
	if (list_index == -1)
		return (-1);
	list[list_index] = NULL;
	return (0);
}

char	**split_semicolons(char *input, t_shell *shell)
{
	char	**list;
	int		count;

	count = parser_count_semicolons(input);
	list = malloc(sizeof(char *) * (count + 2));
	if (!list)
		return (NULL);
	if (fill_segments(list, input, shell) == -1)
	{
		free_split_array(list);
		return (NULL);
	}
	return (list);
}
