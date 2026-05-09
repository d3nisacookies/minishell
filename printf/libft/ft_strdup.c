/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 19:58:29 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 20:10:12 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*temp;
	size_t	i;

	temp = malloc(ft_strlen(s) + 1);
	if (!temp)
	{
		return (NULL);
	}
	i = 0;
	while (s[i])
	{
		temp[i] = s[i];
		i++;
	}
	temp[i] = '\0';
	return (temp);
}

// #include "libft.h"
// #include <stdio.h>

// int	main(void)
// {
// 	char *src = "Hello, Libft!";
// 	char *dup = ft_strdup('\0');

// 	// if (!dup)
// 	// {
// 	// 	printf("ft_strdup failed\n");
// 	// 	return (1);
// 	// }

// 	printf("Original: %s\n", src);
// 	// printf("Duplicate: %s\n", dup);

// 	// dup[0] = 'h';
// 	// printf("Modified duplicate: %s\n", dup);
// 	// printf("Original after modification: %s\n", src);

// 	free(dup);
// 	return (0);
// }