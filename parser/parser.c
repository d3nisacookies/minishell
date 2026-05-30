/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aungk <aungk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 10:00:00 by aungk             #+#    #+#             */
/*   Updated: 2026/05/16 10:00:00 by aungk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_cmd_chain(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		free_cmd(cmd);
		cmd = next;
	}
}

static void	free_pipeline_rest(char **pipeline, int start)
{
	int	i;

	i = start;
	while (pipeline[i])
	{
		free(pipeline[i]);
		i++;
	}
	free(pipeline);
}

static int	append_cmd(t_cmd **head, t_cmd **current, char *segment)
{
	t_cmd	*node;

	node = parse_single(segment);
	if (!node)
		return (-1);
	if (!*head)
		*head = node;
	else
		(*current)->next = node;
	*current = node;
	return (0);
}

static int	build_cmd_list(char **pipeline, t_cmd **head)
{
	t_cmd	*current;
	int		i;

	current = NULL;
	i = 0;
	while (pipeline[i])
	{
		if (append_cmd(head, &current, pipeline[i]) == -1)
		{
			free_pipeline_rest(pipeline, i);
			free_cmd_chain(*head);
			return (-1);
		}
		free(pipeline[i]);
		i++;
	}
	free(pipeline);
	return (0);
}

t_cmd	*parse_command(char *input)
{
	char	**pipeline;
	t_cmd	*head;

	parser_set_status(0);
	if (parser_validate_syntax(input) == -1)
		return (NULL);
	pipeline = split_pipes(input);
	if (!pipeline)
		return (NULL);
	head = NULL;
	if (build_cmd_list(pipeline, &head) == -1)
		return (NULL);
	return (head);
}
