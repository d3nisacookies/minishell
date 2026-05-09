NAME = libftprintf.a

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = ft_printf.c ft_format.c \
       ft_printf_char.c ft_printf_str.c ft_printf_int.c \
       ft_printf_unsigned.c ft_printf_hex.c ft_printf_ptr.c

OBJS = $(SRCS:.c=.o)

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(OBJS)
	make -C libft
	cp $(LIBFT) $(NAME)
	ar rcs $(NAME) $(OBJS)

clean:
	make -C libft clean
	rm -f $(OBJS)

fclean: clean
	make -C libft fclean
	rm -f $(NAME)
	
re: fclean all

.PHONY: all clean fclean re

