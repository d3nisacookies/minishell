/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 00:09:51 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 21:19:16 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static int	get_size(int n)
{
	int	size;

	size = 0;
	if (n <= 0)
		size = 1;
	while (n != 0)
	{
		n /= 10;
		size++;
	}
	return (size);
}

char	*ft_itoa(int n)
{
	char	*res;
	int		size;
	long	num;

	if (n == 0)
		return (ft_strdup("0"));
	if (n == INT_MIN)
		return (ft_strdup("-2147483648"));
	num = n;
	size = get_size(n);
	res = (char *)ft_calloc(size + 1, sizeof(char));
	if (!res)
		return (NULL);
	if (n < 0)
	{
		res[0] = '-';
		num = -num;
	}
	res[size] = '\0';
	while (num > 0)
	{
		res[--size] = (num % 10) + '0';
		num /= 10;
	}
	return (res);
}

// int	main(void)
// {
// 	int n;
// 	char *res;

// 	n = 44221122;
// 	res = ft_itoa(n);
// 	printf("%s\n", res);
// 	free(res);
// 	return (0);
// }