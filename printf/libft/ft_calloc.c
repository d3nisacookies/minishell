/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 19:48:58 by akaung            #+#    #+#             */
/*   Updated: 2025/11/19 21:23:33 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*temp;
	size_t			i;

	i = 0;
	temp = malloc(nmemb * size);
	if (!temp)
	{
		return (NULL);
	}
	while (i < nmemb * size)
	{
		temp[i] = 0;
		i++;
	}
	return (temp);
}

// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	int		*arr;
// 	size_t	n;

// 	n = 5;
// 	arr = ft_calloc(n, sizeof(int));
// 	if (!arr)
// 	{
// 		printf("failure \n");
// 		return (0);
// 	}
// 	printf("created values:\n");
// 	for (int i = 0; i < n; i++)
// 	{
// 		printf("arr[%u] = %d\n", i, arr[i]);
// 	}
// 	free(arr);
// 	return (0);
// }