/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:27:25 by akaung            #+#    #+#             */
/*   Updated: 2025/11/19 19:43:20 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] != '\0' || s2[i] != '\0'))
	{
		if (s1[i] != s2[i])
		{
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		}
		i++;
	}
	return (0);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char str1[] = "ABA";
// 	char str2[] = "AA";
// 	int result;

// 	for (int i = 0; i < sizeof(str2); i++)
// 	{
// 		result = ft_strncmp(str1, str2, i);
// 		printf("result between \"%s\" and \"%s\" at place %d is : %d\n", str1,
// 			str2, i, result);
// 	}
// 	printf("\n");

// 	for (int i = 0; i < sizeof(str2); i++)
// 	{
// 		result = strncmp(str1, str2, i);
// 		printf("result between \"%s\" and \"%s\" at place %d is : %d\n", str1,
// 			str2, i, result);
// 	}

// 	return (0);
// }