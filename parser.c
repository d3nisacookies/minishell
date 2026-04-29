/*
** EPITECH PROJECT, 2026
** minishell
** File description:
** command parser
*/

#include "minishell.h"


static int	count_args(char *input)
{
	int count;
	int in_space;

	count = 0;
	in_space = 1;
	while (*input)
	{
		if (*input == ' ' || *input == '\t')
			in_space = 1;
		else if (in_space)
		{
			count++;
			in_space = 0;
		}
		input++;
	}
	return (count);
}

t_cmd	*parse_command(char *input)
{
	t_cmd *cmd;
	int argc;
	int i;
	char *copy;
	char *token;

	if (input == NULL || strlen(input) == 0)
		return (NULL);
	cmd = malloc(sizeof(t_cmd));
	if (cmd == NULL)
		return (NULL);
	argc = count_args(input);
	cmd->args = malloc(sizeof(char *) * (argc + 1));
	if (cmd->args == NULL)
	{
		free(cmd);
		return (NULL);
	}
	copy = strdup(input);
	token = strtok(copy, " \t");
	i = 0;
	while (token != NULL && i < argc)
	{
		cmd->args[i] = strdup(token);
		token = strtok(NULL, " \t");
		i++;
	}
	cmd->args[i] = NULL;
	cmd->argc = argc;
	free(copy);
	return (cmd);
}
