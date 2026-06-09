CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror -I. -Iprintf -Iprintf/libft
LDFLAGS	=	-lreadline

SRC		=	main.c \
			signals.c \
			prompt.c \
			prompt_utils.c \
			prompt_error_token.c \
			builtins/cd_pwd.c \
			builtins/cd_utils.c \
			builtins/cd_path.c \
			builtins/echo.c \
			builtins/echo_utils.c \
			builtins/export.c \
			builtins/unset.c \
			env/env.c \
			env/env_init.c \
			env/env_remove.c \
			env/env_utils.c \
			executor/executor_expand_arg.c \
			executor/executor_expand.c \
			executor/executor_exit.c \
			executor/executor_exec_error.c \
			executor/executor_free.c \
			executor/executor.c \
			executor/executor_utils.c \
			executor/executor_path.c \
			executor/executor_pipe.c \
			executor/executor_pipe_utils.c \
			executor/executor_pipe_init.c \
			executor/handle_wait_status.c \
			parser/parser.c \
			parser/parser_count_args.c \
			parser/parser_errors.c \
			parser/parser_pipes.c \
			parser/parser_redir_check.c \
			parser/parser_redirections.c \
			parser/parser_semicolon_count.c \
			parser/parser_semicolons.c \
			parser/parser_single.c \
			parser/parser_split_free.c \
			parser/parser_syntax.c \
			parser/parser_utils.c \
			parser/parser_word_utils.c \
			parser/parser_words.c \
			redirections/redirections.c \
			redirections/redirections_io.c \
			redirections/redirections_util.c \

OBJ		=	$(SRC:.c=.o)

NAME		=	minishell
FT_PRINTF	=	printf/libftprintf.a

all:	$(NAME)

$(FT_PRINTF):
	$(MAKE) -C printf

$(NAME):	$(FT_PRINTF) $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(FT_PRINTF) $(LDFLAGS)

$(OBJ):	minishell.h

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
