##
## EPITECH PROJECT, 2026
## minishell
## File description:
## Makefile
##

CC	=	gcc
CFLAGS	=	-Wall -Wextra -Werror -I.
LDFLAGS	=	-lreadline

SRC	=	main.c \
		prompt.c \
		parser.c \
		executor.c

OBJ	=	$(SRC:.c=.o)

NAME	=	minishell

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
