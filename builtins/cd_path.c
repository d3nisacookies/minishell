/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_path.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 15:32:00 by akaung            #+#    #+#             */
/*   Updated: 2026/06/08 15:32:00 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_cd_path(char *base, char *target)
{
	char	*tmp;
	char	*joined;

	if (!target)
		return (NULL);
	if (target[0] == '/')
		return (ft_strdup(target));
	if (!base || base[0] == '\0')
		return (ft_strdup(target));
	tmp = ft_strjoin(base, "/");
	if (!tmp)
		return (NULL);
	joined = ft_strjoin(tmp, target);
	free(tmp);
	return (joined);
}

static int	filter_cd_parts(char **parts)
{
	int	i;
	int	depth;

	i = 0;
	depth = 0;
	while (parts[i])
	{
		if (parts[i][0] == '\0' || ft_strcmp(parts[i], ".") == 0)
			free(parts[i]);
		else if (ft_strcmp(parts[i], "..") == 0)
		{
			free(parts[i]);
			if (depth > 0)
				free(parts[--depth]);
		}
		else
			parts[depth++] = parts[i];
		i++;
	}
	parts[depth] = NULL;
	return (depth);
}

static int	cd_path_len(char **parts, int depth)
{
	int	i;
	int	len;

	len = 1;
	i = 0;
	while (i < depth)
		len += ft_strlen(parts[i++]) + 1;
	return (len);
}

static char	*build_cd_path(char **parts, int depth)
{
	char	*normalized;
	int		i;
	int		pos;

	if (depth == 0)
		return (ft_strdup("/"));
	normalized = malloc(sizeof(char) * cd_path_len(parts, depth));
	if (!normalized)
		return (NULL);
	normalized[0] = '/';
	pos = 1;
	i = 0;
	while (i < depth)
	{
		ft_memcpy(normalized + pos, parts[i], ft_strlen(parts[i]));
		pos += ft_strlen(parts[i]);
		if (i + 1 < depth)
			normalized[pos++] = '/';
		i++;
	}
	normalized[pos] = '\0';
	return (normalized);
}

char	*resolve_cd_pwd(char *oldpwd, char *target)
{
	char	**parts;
	char	*joined;
	char	*newpwd;
	int		depth;

	joined = join_cd_path(oldpwd, target);
	if (!joined)
		return (NULL);
	parts = ft_split(joined, '/');
	free(joined);
	if (!parts)
		return (NULL);
	depth = filter_cd_parts(parts);
	newpwd = build_cd_path(parts, depth);
	free_split_array(parts);
	return (newpwd);
}
