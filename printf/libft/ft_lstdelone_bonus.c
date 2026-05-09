/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 14:37:55 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 15:46:37 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

//  // delete function for content
// static void	del_content(void *content)
// {
// 	free(content);
// }

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!lst)
	{
		return ;
	}
	if (del)
	{
		del(lst->content);
	}
	free(lst);
}

// #include "libft.h"
// #include <stdio.h>
// #include <stdlib.h>

// int	main(void)
// {
// 	t_list	*head;
// 	t_list	*tmp;

// 	head = NULL;
// 	t_list *n1, *n2, *n3;
// 	// create nodes with dynamically allocated strings
// 	n1 = ft_lstnew(ft_strdup("Hello"));
// 	n2 = ft_lstnew(ft_strdup("World"));
// 	n3 = ft_lstnew(ft_strdup("Welcome"));
// 	// build the list: head -> n1 -> n2 -> n3
// 	ft_lstadd_back(&head, n1);
// 	ft_lstadd_back(&head, n2);
// 	ft_lstadd_back(&head, n3);
// 	// print the list before deletion
// 	printf("Before deletion:\n");
// 	tmp = head;
// 	while (tmp)
// 	{
// 		printf("%s\n", (char *)tmp->content);
// 		tmp = tmp->next;
// 	}
// 	// delete the second node (n2)
// 	head->next = n2->next; // bypass n2
// 	ft_lstdelone(n2, del_content);
// 	// print the list after deletion
// 	printf("\nAfter deletion of second node:\n");
// 	tmp = head;
// 	while (tmp)
// 	{
// 		printf("%s\n", (char *)tmp->content);
// 		tmp = tmp->next;
// 	}
// 	// clean up the remaining list
// 	ft_lstdelone(n1, del_content);
// 	ft_lstdelone(n3, del_content);
// 	return (0);
// }