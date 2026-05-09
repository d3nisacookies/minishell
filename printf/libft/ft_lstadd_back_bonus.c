/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 14:25:36 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 14:37:20 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!*lst)
	{
		*lst = new;
	}
	else
	{
		last = ft_lstlast(*lst);
		last->next = new;
	}
}

// #include <stdio.h>

// int	main(void)
// {
// 	t_list	*head;
// 	t_list	*n1;
// 	t_list	*n2;
// 	t_list	*n3;
// 	t_list	*res;
// 	t_list	*tmp;

// 	head = NULL;
// 	n1 = ft_lstnew("Hello ");
// 	n2 = ft_lstnew("World! ");
// 	n3 = ft_lstnew("Welcome.");
// 	// Add nodes to the front
// 	ft_lstadd_front(&head, n2);
// 	ft_lstadd_front(&head, n1);
// 	// Get last node
// 	res = ft_lstlast(head);
// 	printf("Current list:\n");
// 	tmp = head; // use temporary pointer for traversal
// 	while (tmp != NULL)
// 	{
// 		printf("%s\n", (char *)tmp->content);
// 		tmp = tmp->next;
// 	}
// 	printf("Last node is: %s\n", (char *)res->content);
// 	// Add node to the back
// 	ft_lstadd_back(&head, n3);
// 	printf("After adding to the back:\n");
// 	tmp = head; // reset tmp to start of list
// 	while (tmp != NULL)
// 	{
// 		printf("%s\n", (char *)tmp->content);
// 		tmp = tmp->next;
// 	}
// 	// Last node is now n3
// 	res = ft_lstlast(head);
// 	printf("Last node is: %s\n", (char *)res->content);
// 	return (0);
// }