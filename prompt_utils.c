/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:44:59 by akaung            #+#    #+#             */
/*   Updated: 2026/06/02 12:45:07 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*trim_spaces(char *s)
{
	int	start;
	int	end;

	if (!s)
		return (NULL);
	start = 0;
	while (s[start] && parser_is_space(s[start]))
		start++;
	end = ft_strlen(s);
	while (end > start && parser_is_space(s[end - 1]))
		end--;
	s[end] = '\0';
	return (s + start);
}

int	is_trailing_empty_segment(char **segments, int index)
{
	if (index == 0)
		return (0);
	if (segments[index + 1] != NULL)
		return (0);
	return (1);
}

int	ends_with_pipe(char *segment)
{
	int	end;

	end = ft_strlen(segment);
	while (end > 0 && parser_is_space(segment[end - 1]))
		end--;
	if (end == 0)
		return (0);
	return (segment[end - 1] == '|');
}

int	starts_with_pipe(char *segment)
{
	int	start;

	start = 0;
	while (segment[start] && parser_is_space(segment[start]))
		start++;
	return (segment[start] == '|');
}

int	handle_parse_fail(t_shell *shell, char **segments)
{
	int	status;

	status = parser_get_status(shell);
	if (!status)
		status = 1;
	shell->last_exit = status;
	free_split_array(segments);
	return (-1);
}
