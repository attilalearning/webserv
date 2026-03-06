
NAME = webserve

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(IDIR) -g -O0

SRCS = Config.cpp main.cpp Server.cpp Utils.cpp



ODIR = objs/
SDIR = sources/
IDIR = includes/
FULL_SRCS = $(addprefix $(SDIR), $(SRCS))
OBJS = $(patsubst $(SDIR)%.c, $(ODIR)%.o, $(FULL_SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(ODIR)%.o : $(SDIR)%.cpp | $(ODIR)
	$(CC) $(CFLAGS) $< -c -o $@


clean:
	rm -rf $(ODIR)

fclean: clean
	rm $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re