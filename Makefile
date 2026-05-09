CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror -I. -Iprintf -Iprintf/libft
LDFLAGS	=	-lreadline

SRC		=	main.c \
            prompt.c \
            parser.c \
            executor.c \
            env.c \
            builtins.c

OBJ		=	$(SRC:.c=.o)

NAME		=	minishell
FT_PRINTF	=	printf/libftprintf.a

all:	$(NAME)

$(FT_PRINTF):
	$(MAKE) -C printf

$(NAME):	$(FT_PRINTF) $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(FT_PRINTF) $(LDFLAGS)

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
	$(MAKE) -C printf clean

fclean:	clean
	rm -f $(NAME)
	$(MAKE) -C printf fclean

re:	fclean all

.PHONY:	all clean fclean re
