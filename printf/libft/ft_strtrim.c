/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 21:13:30 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 21:33:35 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	to_trim(const char *set, char c)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	size_t	start;
	size_t	end;
	char	*res;
	size_t	len;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	end = ft_strlen(s1);
	if (end == 0)
		return (ft_strdup(""));
	end--;
	while (start <= end && to_trim(set, s1[start]))
		start++;
	while (end >= start && to_trim(set, s1[end]))
		end--;
	len = 0;
	if (start <= end)
		len = (end - start + 1);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	ft_strlcpy(res, s1 + start, len + 1);
	return (res);
}

// #include "libft.h"
// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	char *s1;
// 	char *set;
// 	char *res;

// 	// Basic test
// 	s1 = "+++Hello World!!!+++";
// 	set = "+!";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	// Test with spaces
// 	s1 = "   \t  Hello there!  \n ";
// 	set = " \n\t";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	// Test: no trimming needed
// 	s1 = "abcdef";
// 	set = "xyz";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	// Test: string is fully trimmed
// 	s1 = "aaaaaa";
// 	set = "a";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	// Test: empty string
// 	s1 = "";
// 	set = "abc";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	// Test: trimming nothing
// 	s1 = "Hello";
// 	set = "";
// 	res = ft_strtrim(s1, set);
// 	printf("Input: \"%s\"\n", s1);
// 	printf("Trim set: \"%s\"\n", set);
// 	printf("Result: \"%s\"\n\n", res);
// 	free(res);
// 	return (0);
// }