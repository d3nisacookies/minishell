/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 20:27:56 by akaung            #+#    #+#             */
/*   Updated: 2025/11/19 21:06:09 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*res;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	res = malloc((ft_strlen(s1) + ft_strlen(s2)) + 1);
	if (!res)
		return (NULL);
	while (i < ft_strlen(s1))
	{
		res[i] = s1[i];
		i++;
	}
	while (j < ft_strlen(s2))
	{
		res[i + j] = s2[j];
		j++;
	}
	res[i + j] = '\0';
	return (res);
}

// #include "libft.h"
// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	char	*s1;
// 	char	*s2;
// 	char	*res;

// 	s1 = "Hello, ";
// 	s2 = "World!";
// 	res = ft_strjoin(s1, s2);
// 	if (!res)
// 	{
// 		printf("ft_strjoin failed\n");
// 		return (1);
// 	}
// 	printf("Result: %s\n", res);
// 	free(res);
// 	// Additional tests
// 	printf("\n--- Additional Tests ---\n");
// 	res = ft_strjoin("", "Test");
// 	printf("Empty + Test = %s\n", res);
// 	free(res);
// 	res = ft_strjoin("Test", "");
// 	printf("Test + Empty = %s\n", res);
// 	free(res);
// 	res = ft_strjoin("", "");
// 	printf("Empty + Empty = %s\n", res);
// 	free(res);
// 	return (0);
// }