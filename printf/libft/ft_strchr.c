/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 13:54:20 by akaung            #+#    #+#             */
/*   Updated: 2025/11/20 02:28:59 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == (char)c)
		{
			return ((char *)&s[i]);
		}
		i++;
	}
	if ((char)c == '\0')
	{
		return ((char *)&s[i]);
	}
	return (NULL);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	char myString[] = "Hello, world!";
// 	char *result;

// 	result = ft_strchr(myString, 'o');
// 	if (result != NULL)
// 	{
// 		printf("First 'o' found at: %s\n", result);
// 	}
// 	else
// 	{
// 		printf("'o' not found.\n");
// 	}

// 	result = ft_strchr(myString, 'x');
// 	if (result != NULL)
// 	{
// 		printf("First 'x' found at: %s\n", result);
// 	}
// 	else
// 	{
// 		printf("'x' not found.\n");
// 	}

// 	return (0);
// }