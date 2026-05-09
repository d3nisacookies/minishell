/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_format.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:39:22 by akaung            #+#    #+#             */
/*   Updated: 2026/01/12 10:39:39 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_format(const char c, va_list args)
{
	if (c == 'c')
		return (ft_printf_char(va_arg(args, int)));
	if (c == 's')
		return (ft_printf_str(va_arg(args, char *)));
	if (c == 'd' || c == 'i')
		return (ft_printf_int(va_arg(args, int)));
	if (c == 'p')
		return (ft_printf_ptr(va_arg(args, unsigned long long)));
	if (c == 'u')
		return (ft_printf_unsigned(va_arg(args, unsigned int)));
	if (c == 'x' || c == 'X')
		return (ft_printf_hex(va_arg(args, unsigned int), c));
	if (c == '%')
		return (write(1, "%", 1));
	return (0);
}
