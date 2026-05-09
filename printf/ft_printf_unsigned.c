/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_unsigned.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:40:23 by akaung            #+#    #+#             */
/*   Updated: 2026/01/12 10:40:24 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	put_unsigned(unsigned int n)
{
	int	count;
	int	digit;

	count = 0;
	if (n >= 10)
	{
		count += put_unsigned(n / 10);
	}
	digit = (n % 10) + '0';
	count += write(1, &digit, 1);
	return (count);
}

int	ft_printf_unsigned(unsigned int n)
{
	return (put_unsigned(n));
}
