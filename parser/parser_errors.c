#include "minishell.h"

void	parser_put_unmatched_quote_error(void)
{
	parser_set_status(2);
	ft_putstr_fd("minishell: syntax error: unmatched quote\n", 2);
}

void	parser_put_pipe_error(void)
{
	parser_set_status(2);
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
}
