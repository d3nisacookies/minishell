/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:41:10 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 16:54:40 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (ptr[i] == (unsigned char)c)
		{
			return ((unsigned char *)(ptr + i));
		}
		i++;
	}
	return (NULL);
}

// #include <stdio.h>
// #include <string.h>

// int	main(void)
// {
// 	const char str[] = "ABCDEFG";
// 	const int chars[] = {'\0', '\0'};
// 	for (size_t i = 0; i < sizeof chars / (sizeof chars[0]); ++i)
// 	{
// 		const int c = chars[i];
// 		const char *ps = ft_memchr(((void *)0), '\0', 0x20);
// 		ps ? printf("character '%c'(%i) found: %s\n", c, c,
// 			ps) : printf("character '%c'(%i) not found\n", c, c);
// 	}
// 	return (0);
// }