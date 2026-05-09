/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 21:36:21 by akaung            #+#    #+#             */
/*   Updated: 2025/11/20 00:02:38 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_delim(char c, char delim)
{
	return (c == delim);
}

static size_t	count_word(const char *s, char c)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (s[i] != '\0')
	{
		while (s[i] && is_delim(s[i], c))
			i++;
		if (s[i] && !is_delim(s[i], c))
			count++;
		while (s[i] && !is_delim(s[i], c))
			i++;
	}
	return (count);
}

char	**ft_split(const char *s, char c)
{
	char		**res;
	size_t		i;
	const char	*word_start;

	if (!s)
		return (NULL);
	res = malloc((count_word(s, c) + 1) * sizeof(char *));
	if (!res)
		return (NULL);
	i = 0;
	while (*s)
	{
		while (*s && is_delim(*s, c))
			s++;
		if (!*s)
			break ;
		word_start = s;
		while (*s && !is_delim(*s, c))
			s++;
		res[i] = ft_substr(word_start, 0, s - word_start);
		i++;
	}
	res[i] = NULL;
	return (res);
}

// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	char	**words;
// 	char	*str;
// 	char	delimiter;
// 	size_t	i;

// 	str = "Hello,,42,,School!";
// 	delimiter = ',';
// 	words = ft_split(str, delimiter);
// 	if (!words)
// 	{
// 		printf("ft_split returned NULL\n");
// 		return (1);
// 	}
// 	i = 0;
// 	while (words[i])
// 	{
// 		printf("words[%zu] = \"%s\"\n", i, words[i]);
// 		free(words[i]); // Free each allocated word
// 		i++;
// 	}
// 	free(words); // Free the array itself
// 	return (0);
// }
