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
