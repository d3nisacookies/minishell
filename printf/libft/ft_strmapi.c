/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 01:19:15 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 16:28:59 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(const char *s, char (*f)(unsigned int, char))
{
	unsigned int	i;
	char			*result;

	result = malloc((ft_strlen(s) + 1) * sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	while (s[i] != '\0')
	{
		result[i] = (*f)(i, s[i]);
		i++;
	}
	result[i] = '\0';
	return (result);
}

// #include "libft.h"
// #include <ctype.h>
// #include <stdio.h>

// /* Example mapping function: uppercase everything */
// char	to_upper(unsigned int i, char c)
// {
// 	(void)i;
// 	return (char)toupper((unsigned char)c);
// }

// /* Example: shift each char by its index */
// char	shift_by_index(unsigned int i, char c)
// {
// 	return (c + i);
// }

// /* Example: alternate between uppercase / lowercase */
// char	alternate_case(unsigned int i, char c)
// {
// 	if (i % 2 == 0)
// 		return (char)toupper((unsigned char)c);
// 	return (char)tolower((unsigned char)c);
// }

// int	main(void)
// {
// 	char	*original;
// 	char	*result;

// 	original = "Hello42World";
// 	// Test 1: Uppercase
// 	result = ft_strmapi(original, &to_upper);
// 	printf("Upper:      %s\n", result);
// 	free(result);
// 	// Test 2: Shift by index
// 	result = ft_strmapi(original, &shift_by_index);
// 	printf("Shifted:    %s\n", result);
// 	free(result);
// 	// Test 3: Alternate case
// 	result = ft_strmapi(original, &alternate_case);
// 	printf("Alt case:   %s\n", result);
// 	free(result);
// 	return (0);
// }
