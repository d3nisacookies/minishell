/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 17:41:53 by akaung            #+#    #+#             */
/*   Updated: 2025/11/20 02:31:48 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *des, const char *src, size_t size)
{
	size_t	src_len;
	size_t	i;

	src_len = ft_strlen(src);
	if (size == 0)
	{
		return (src_len);
	}
	i = 0;
	while (src[i] && i < (size - 1))
	{
		des[i] = src[i];
		i++;
	}
	des[i] = '\0';
	return (src_len);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char src[] = "Hello, World!";
// 	char dest[10];
// 	size_t result;

// 	result = ft_strlcpy(dest, src, sizeof(dest));

// 	printf("destination: '%s'\n", dest);
// 	printf("length: %zu\n", result);
// 	printf("truncated: %s\n", result > sizeof(dest) - 1 ? "Yes" : "No");
// 	return (0);
// }