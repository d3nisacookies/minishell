#include "minishell.h"

int	parser_is_redirection(char *word)
{
	if (!word)
		return (0);
	if (ft_strcmp(word, ">") == 0)
		return (1);
	if (ft_strcmp(word, ">>") == 0)
		return (1);
	if (ft_strcmp(word, "<") == 0)
		return (1);
	if (ft_strcmp(word, "<<") == 0)
		return (1);
	return (0);
}
