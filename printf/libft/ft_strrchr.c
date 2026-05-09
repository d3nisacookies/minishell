/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:11:49 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 16:18:39 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int	i;

	i = ft_strlen(s);
	while (i >= 0)
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i--;
	}
	return (NULL);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char str[] = "Hello world";
// 	char *result;

// 	result = ft_strrchr(str, 'x');
// 	if (result != NULL)
// 	{
// 		printf("letter found at : %s", result);
// 	}
// 	else
// 	{
// 		printf("letter not found: %s", result);
// 	}
// 	return (0);
// }