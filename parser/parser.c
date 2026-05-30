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

static int	validate_syntax(char *input);

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
	if (validate_syntax(input) == -1)
		return (NULL);
	pipeline = split_pipes(input);
	if (!pipeline)
		return (NULL);
	head = NULL;
	if (build_cmd_list(pipeline, &head) == -1)
		return (NULL);
	return (head);
}

static int	validate_syntax(char *input)
{
	int		i;
    char	quote;

	i = 0;
	quote = 0;
	while (input[i])
	{
		if (!quote && (input[i] == '\'' || input[i] == '"'))
			quote = input[i];
		else if (quote && input[i] == quote)
			quote = 0;
		i++;
	}
	if (quote)
		return (parser_put_unmatched_quote_error(), -1);
	return (0);
}

static int	count_semicolons(char *input)
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

static int	fill_segments(char **list, char *input)
{
	int		i;
	int		start;
	int		count;
	char	quote;

	i = 0;
	start = 0;
	count = 0;
	quote = 0;
	while (input[i])
	{
		if (quote == 0 && (input[i] == '\'' || input[i] == '"'))
			quote = input[i];
		else if (quote != 0 && input[i] == quote)
			quote = 0;
		else if (quote == 0 && input[i] == ';')
		{
			list[count++] = ft_substr(input, start, i - start);
			if (!list[count - 1])
				return (-1);
			start = i + 1;
		}
		i++;
	}
	if (quote != 0)
		return (parser_put_unmatched_quote_error(), -1);
	list[count++] = ft_substr(input, start, i - start);
	if (!list[count - 1])
		return (-1);
	list[count] = NULL;
	return (0);
}

char	**split_semicolons(char *input)
{
	char	**list;
	int		count;

	count = count_semicolons(input);
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

void	free_split_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
