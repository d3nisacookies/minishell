/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:40:43 by akaung            #+#    #+#             */
/*   Updated: 2026/01/12 10:40:43 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

int	ft_printf(const char *format, ...);
int	ft_format(const char c, va_list args);

int	ft_printf_char(char c);
int	ft_printf_str(char *s);
int	ft_printf_int(int n);
int	ft_printf_unsigned(unsigned int n);
int	ft_printf_hex(unsigned int n, char format);
int	ft_printf_ptr(unsigned long long ptr);

#endif
