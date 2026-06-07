/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_expand_arg.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 02:34:00 by Copilot           #+#    #+#             */
/*   Updated: 2026/06/08 02:34:00 by Copilot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*append_str(char *dst, char *src)
{
	char	*tmp;

	if (!src)
		return (dst);
	if (!dst)
		return (ft_strdup(src));
	tmp = ft_strjoin(dst, src);
	free(dst);
	return (tmp);
}

static int	expand_variable(t_shell *shell, char *arg, int *i, char **result)
{
	char	*key;
	int		start;

	start = ++(*i);
	while (arg[*i] && is_var_char(arg[*i]))
		(*i)++;
	key = ft_substr(arg, start, *i - start);
	if (!key)
		return (0);
	*result = append_str(*result, get_env_value(shell, key));
	free(key);
	return (*result != NULL);
}

static int	expand_dollar(t_shell *shell, char *arg, int *i, char **result)
{
	char	*chunk;

	if (arg[*i + 1] == '?')
	{
		chunk = ft_itoa(shell->last_exit);
		*result = append_str(*result, chunk);
		free(chunk);
		*i += 2;
		return (*result != NULL);
	}
	if (!is_var_start(arg[*i + 1]))
	{
		*result = append_str(*result, "$");
		(*i)++;
		return (*result != NULL);
	}
	return (expand_variable(shell, arg, i, result));
}

char	*expand_argument(t_shell *shell, char *arg, int quote)
{
	char	*result;
	char	chr[2];
	int		i;

	if (!arg || quote == '\'')
		return (ft_strdup(arg));
	result = ft_strdup("");
	i = 0;
	while (result && arg[i])
	{
		if (arg[i] != '$')
		{
			chr[0] = arg[i++];
			chr[1] = '\0';
			result = append_str(result, chr);
		}
		else if (!expand_dollar(shell, arg, &i, &result))
			break ;
	}
	return (result);
}
