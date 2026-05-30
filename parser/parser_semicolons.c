#include "minishell.h"

static int	quote_state_changed(char *quote, char c)
{
	if (*quote == 0 && (c == '\'' || c == '"'))
		return (*quote = c, 1);
	if (*quote != 0 && c == *quote)
		return (*quote = 0, 1);
	return (0);
}

static int	append_segment(char **list, int *count, char *input, int *range)
{
	list[*count] = ft_substr(input, range[0], range[1] - range[0]);
	if (!list[*count])
		return (-1);
	(*count)++;
	return (0);
}

static int	fill_loop(char **list, char *input, int *range, char *quote)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (!quote_state_changed(quote, input[i])
			&& *quote == 0 && input[i] == ';'
			&& (range[1] = i) >= 0
			&& append_segment(list, &range[2], input, range) == -1)
			return (-1);
		if (*quote == 0 && input[i] == ';')
			range[0] = i + 1;
		i++;
	}
	range[1] = i;
	return (0);
}

static int	fill_segments(char **list, char *input)
{
	int		range[3];
	char	quote;

	range[0] = 0;
	range[2] = 0;
	quote = 0;
	if (fill_loop(list, input, range, &quote) == -1)
		return (-1);
	if (quote != 0)
		return (parser_put_unmatched_quote_error(), -1);
	if (append_segment(list, &range[2], input, range) == -1)
		return (-1);
	list[range[2]] = NULL;
	return (0);
}

char	**split_semicolons(char *input)
{
	char	**list;
	int		count;

	count = parser_count_semicolons(input);
	list = malloc(sizeof(char *) * (count + 2));
	if (!list)
		return (NULL);
	if (fill_segments(list, input) == -1)
	{
		free_split_array(list);
		return (NULL);
	}
	return (list);
}
