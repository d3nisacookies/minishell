/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:40:32 by akaung            #+#    #+#             */
/*   Updated: 2026/01/28 14:04:45 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

//#include <stdio.h>

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;

	va_start(args, format);
	count = 0;
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			count += ft_format(*format, args);
		}
		else
		{
			count += write(1, format, 1);
		}
		format++;
	}
	va_end(args);
	return (count);
}

// #include <stdio.h>

// int	main(void)
// {
// 	const char	*fmt_char = "[%c]\n";
// 	const char	*fmt_str = "[%s]\n";
// 	const char	*fmt_int = "[%d]\n";
// 	const char	*fmt_uns = "[%u]\n";
// 	const char	*fmt_hex = "[%x]\n";
// 	const char	*fmt_heX = "[%X]\n";
// 	const char	*fmt_pct = "[%%]\n";
// 	void		*p;

// 	// const char	*fmt_ptr = "[%p]\n";
// 	// int			x;
// 	int r1, r2;
// 	// Characters
// 	r1 = printf(fmt_char, 'A');
// 	r2 = ft_printf(fmt_char, 'A');
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Strings
// 	r1 = printf(fmt_str, "Hello World");
// 	r2 = ft_printf(fmt_str, "Hello World");
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Integers
// 	r1 = printf(fmt_int, -12345);
// 	r2 = ft_printf(fmt_int, -12345);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Unsigned
// 	r1 = printf(fmt_uns, 3000000000u);
// 	r2 = ft_printf(fmt_uns, 3000000000u);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Hex lowercase
// 	r1 = printf(fmt_hex, 3735928559u);
// 	r2 = ft_printf(fmt_hex, 3735928559u);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Hex uppercase
// 	r1 = printf(fmt_heX, 3735928559u);
// 	r2 = ft_printf(fmt_heX, 3735928559u);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Pointer
// 	p = NULL;
// 	r1 = printf("ptr: %p\n", p);
// 	r2 = ft_printf("ptr: %p\n", p);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	// Percent
// 	r1 = printf("%s", fmt_pct);
// 	r2 = ft_printf("%s", fmt_pct);
// 	printf("ret printf = %d | ret ft_printf = %d\n\n", r1, r2);
// 	return (0);
// }
