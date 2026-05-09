/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 16:58:23 by akaung            #+#    #+#             */
/*   Updated: 2025/11/18 17:05:51 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
}
// #include <stdio.h>

// int	main(void)
// {
// 	char	a;
// 	char	b;
// 	char	c;

// 	a = 'a';
// 	b = '1';
// 	c = '$';
// 	if (ft_isalnum(c))
// 	{
// 		printf("this is alnum.");
// 	}
// 	else
// 	{
// 		printf("not alnum.");
// 	}
// 	return (0);
// }
