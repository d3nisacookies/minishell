/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 14:10:10 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 14:21:29 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	if (!lst)
	{
		return (NULL);
	}
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

// #include <stdio.h>

// int	main(void)
// {
// 	t_list *head;
// 	t_list *n1;
// 	t_list *n2;
// 	t_list *n3;
// 	t_list *res;

// 	head = NULL;
// 	n1 = ft_lstnew("Hello ");
// 	n2 = ft_lstnew("World! ");
// 	n3 = ft_lstnew("Welcome.");

// 	ft_lstadd_front(&head, n3);
// 	ft_lstadd_front(&head, n2);
// 	ft_lstadd_front(&head, n1);
// 	res = ft_lstlast(head);

// 	while (head != NULL)
// 	{
// 		printf("%s\n", (char *)head->content);
// 		head = head->next;
// 	}

// 	// res = ft_lstlast(head);
// 	printf("last node is: %s\n", (char *)res->content);

// 	return (0);
// }