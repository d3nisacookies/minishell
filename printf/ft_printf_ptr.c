/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_ptr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:40:12 by akaung            #+#    #+#             */
/*   Updated: 2026/01/28 14:02:14 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	put_ptr(unsigned long long n)
{
	char	*base;
	int		count;

	base = "0123456789abcdef";
	count = 0;
	if (n >= 16)
	{
		count += put_ptr(n / 16);
	}
	count += write(1, &base[n % 16], 1);
	return (count);
}

int	ft_printf_ptr(unsigned long long ptr)
{
	int	count;

	if (ptr == 0)
	{
		return (write(1, "(nil)", 5));
	}
	count = 0;
	count += write(1, "0x", 2);
	count += put_ptr(ptr);
	return (count);
}
