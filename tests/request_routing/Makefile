# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aistok <aistok@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/25 10:02:48 by aistok            #+#    #+#              #
#    Updated: 2026/04/25 10:13:06 by aistok           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98
#DFLAGS		=	-g3 -O0
#DFLAGS		=	-fsanitise=address
RM			=	rm -rf

#INC_DIR		=	./inc
SRC_DIR		=	./src
BIN_DIR		=	./bin

NAME		=	$(BIN_DIR)/cat_request

SRC_FILES	=	$(SRC_DIR)/cat_request.cpp

all: $(NAME)

$(NAME): $(SRC_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC_FILES) -o $@

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_FILES) | $(OBJ_DIR)
#	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

#$(OBJ_DIR):
#	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	@echo clean ok
#	$(RM) $(OBJ_DIR)

fclean: clean
	@echo fclean ok
	$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all clean fclean re
