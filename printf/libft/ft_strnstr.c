/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 17:10:17 by akaung            #+#    #+#             */
/*   Updated: 2025/11/19 19:44:57 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	if (*little == '\0')
	{
		return ((char *)big);
	}
	i = 0;
	while (i < len && big[i])
	{
		j = 0;
		while (big[i + j] && little[j] && i + j < len && big[i
				+ j] == little[j])
		{
			j++;
		}
		if (!little[j])
		{
			return ((char *)&big[i]);
		}
		i++;
	}
	return (NULL);
}

// #include <bsd/string.h>
// #include <stdio.h>

// int	main(void)
// {
// 	const char *text = "This is a test string.";
// 	const char *search = "test";
// 	const char *result;

// 	// Search within the first 10 characters
// 	result = ft_strnstr(text, search, 10);
// 	if (result != NULL)
// 	{
// 		printf("Found '%s' within the first 10 characters at: %s\n", search,
// 			result);
// 	}
// 	else
// 	{
// 		printf("'%s' not found within the first 10 characters.\n", search);
// 	}

// 	// Search within the entire string (or a larger limit)
// 	result = ft_strnstr(text, search, strlen(text));
// 	if (result != NULL)
// 	{
// 		printf("Found '%s' in the full string at: %s\n", search, result);
// 	}
// 	else
// 	{
// 		printf("'%s' not found in the full string.\n", search);
// 	}

// 	return (0);
// }