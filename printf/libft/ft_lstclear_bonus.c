/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 15:03:43 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 19:12:27 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*tmp;

	if (!lst || !del)
	{
		return ;
	}
	while (*lst)
	{
		tmp = (*lst)->next;
		ft_lstdelone(*lst, del);
		*lst = tmp;
	}
}

// #include <stdio.h>

// int	main(void)
// {
// 	t_list *head = NULL;

// 	ft_lstadd_back(&head, ft_lstnew(ft_strdup("Hello")));
// 	ft_lstadd_back(&head, ft_lstnew(ft_strdup("World")));
// 	ft_lstadd_back(&head, ft_lstnew(ft_strdup("Welcome")));

// 	ft_lstclear(&head, free);
// 	// deletes all nodes and their content
// 	printf("head is now %p\n", (void *)head); // prints 0 / NULL
// }