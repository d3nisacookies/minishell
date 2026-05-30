/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir_check.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 17:28:18 by akaung            #+#    #+#             */
/*   Updated: 2026/05/30 17:28:19 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
