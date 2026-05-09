/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 18:20:15 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 21:03:14 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// static int	isspace(int c)
// {
// 	if (c == 9 || c == 10 || c == 11 || c == 12 || c == 13 || c == 32)
// 	{
// 		return (1);
// 	}
// 	return (0);
// }

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sign;
	int	result;

	i = 0;
	sign = 1;
	result = 0;
	while (nptr[i] && (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13)))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] && nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}

// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	char str1[] = "12345";
// 	char str2[] = " -6789";
// 	char str3[] = "abc123";
// 	char str4[] = "   a+987";

// 	int num1 = ft_atoi(str1);
// 	int num2 = ft_atoi(str2);
// 	int num3 = ft_atoi(str3);
// 	int num4 = ft_atoi(str4);

// 	printf("String \"%s\" converted to integer: %d\n", str1, num1);
// 	printf("String \"%s\" converted to integer: %d\n", str2, num2);
// 	printf("String \"%s\" converted to integer: %d\n", str3, num3);
// 	printf("String \"%s\" converted to integer: %d\n", str4, num4);

// 	return (0);
// }