/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:15 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:28:16 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_pipes(char *s)
{
	int		i;
	int		count;
	char	quote;

	i = 0;
	count = 1;
	quote = 0;
	while (s[i])
	{
		if (!quote && (s[i] == '\'' || s[i] == '"'))
			quote = s[i];
		else if (quote && s[i] == quote)
			quote = 0;
		else if (!quote && s[i] == '|')
			count++;
		i++;
	}
	if (quote)
		return (-1);
	return (count);
}

static int	push_segment(char **res, int *j, char *input, int *range)
{
	int	start;
	int	end;

	start = range[0];
	end = range[1];
	while (start < end)
	{
		if (!parser_is_space(input[start]))
			break ;
		start++;
	}
	if (start == end)
		return (-1);
	res[*j] = ft_substr(input, start, end - start);
	if (!res[*j])
		return (-1);
	(*j)++;
	return (0);
}

static int	is_pipe_split(char *input, int i, char *quote)
{
	if (!*quote && (input[i] == '\'' || input[i] == '"'))
	{
		*quote = input[i];
		return (0);
	}
	if (*quote && input[i] == *quote)
	{
		*quote = 0;
		return (0);
	}
	if (!*quote && input[i] == '|')
		return (1);
	return (0);
}

static int	split_loop(char *input, char **res, int *j)
{
	int		i;
	int		start;
	int		range[2];
	char	quote;

	i = 0;
	start = 0;
	quote = 0;
	while (input[i])
	{
		if (is_pipe_split(input, i, &quote))
		{
			range[0] = start;
			range[1] = i;
			if (push_segment(res, j, input, range) == -1)
				return (-1);
			start = i + 1;
		}
		i++;
	}
	range[0] = start;
	range[1] = i;
	if (push_segment(res, j, input, range) == -1)
		return (-1);
	return (0);
}

char	**split_pipes(char *input, t_shell *shell)
{
	char	**res;
	int		j;
	int		pipes;

	pipes = count_pipes(input);
	if (pipes < 0)
		return (parser_put_unmatched_quote_error(shell), NULL);
	res = malloc(sizeof(char *) * (pipes + 1));
	if (!res)
		return (NULL);
	j = 0;
	if (split_loop(input, res, &j) == -1)
		return (parser_free_split(res, j), parser_put_pipe_error(shell), NULL);
	res[j] = NULL;
	return (res);
}
