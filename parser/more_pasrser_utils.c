/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   more_pasrser_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 18:02:12 by akaung            #+#    #+#             */
/*   Updated: 2026/06/02 18:02:14 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parser_redirection_error(t_shell *shell, char *op)
{
	parser_set_status(shell, 2);
	ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n",
		2);
	free(op);
	return (-1);
}