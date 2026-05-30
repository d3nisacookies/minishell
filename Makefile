CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror -I. -Iprintf -Iprintf/libft
LDFLAGS	=	-lreadline

SRC		=	main.c \
			prompt.c \
			prompt_utils.c \
			builtins/cd_pwd.c \
			builtins/cd_utils.c \
			builtins/echo.c \
			builtins/echo_utils.c \
			builtins/export.c \
			builtins/unset.c \
			env/env.c \
			env/env_utils.c \
			executor/executor_expand.c \
			executor/executor_exit.c \
			executor/executor_exec_error.c \
			executor/executor_free.c \
			executor/executor.c \
			executor/executor_pipe.c \
			parser/parser.c \
			parser/parser_pipes.c \
			parser/parser_redirections.c \
			parser/parser_single.c \
			parser/parser_utils.c \
			parser/parser_words.c \
			redirections/redirections.c

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
	find . -type f -name "*.o" -not -path "./printf/*" -delete
	$(MAKE) -C printf clean

fclean:	clean
	rm -f $(NAME)
	$(MAKE) -C printf fclean

re:	fclean all

.PHONY:	all clean fclean re
