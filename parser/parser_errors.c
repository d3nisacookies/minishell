/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_errors.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:10 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:28:11 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parser_put_unmatched_quote_error(t_shell *shell)
{
	parser_set_status(shell, 2);
	ft_putstr_fd("minishell: syntax error: unmatched quote\n", 2);
}

void	parser_put_pipe_error(t_shell *shell)
{
	parser_set_status(shell, 2);
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
}
