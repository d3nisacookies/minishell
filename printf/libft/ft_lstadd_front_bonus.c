/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akaung <akaung@student.42.sg>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 13:43:44 by akaung            #+#    #+#             */
/*   Updated: 2025/11/21 19:07:19 by akaung           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!lst || !new)
	{
		return ;
	}
	new->next = *lst;
	*lst = new;
}

// #include <stdio.h>

// int	main(void)
// {
// 	t_list *head;
// 	t_list *n1;
// 	t_list *n2;

// 	head = NULL;
// 	n1 = ft_lstnew("Hello ");
// 	n2 = ft_lstnew("World !");

// 	ft_lstadd_front(&head, n2);
// 	ft_lstadd_front(&head, n1);

// 	printf("list 1 : %s\nlist 2 : %s\n", (char *)head->content,
// 		(char *)head->next->content);
// 	return (0);
// }