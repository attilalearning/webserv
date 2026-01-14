# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aistok <aistok@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/14 18:42:19 by aistok            #+#    #+#              #
#    Updated: 2026/01/14 19:26:07 by aistok           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98
#DFLAGS		=	-fsanitise=address
RM			=	rm -rf

INC_DIR		=	./inc
SRC_DIR		=	./src
OBJ_DIR		=	./obj
BIN_DIR		=	./bin

NAME		=	$(BIN_DIR)/webserv

# TO GET THE ALL SOURCE FILES (TEMPORARILY), UNTIL THE PROJECT BECOMES MORE STABLE
TMP_SRC_DIRS	= ${shell find ${SRC_DIR} -type d}
TMP_OBJ_DIRS	= ${subst ${SRC_DIR},${OBJ_DIR},${TMP_SRC_DIRS}}
TMP_SRC_FILES	= ${wildcard ${addsuffix /*, ${TMP_SRC_DIRS}}}
INC_FILES		= ${shell find ${INC_DIR} -name "*.hpp"}
SRC_FILES		= ${filter %.cpp, $(TMP_SRC_FILES)}

# COMMENTED UNTIL CLOSE TO THE FINAL STAGES
# INC_FILES	=	$(INC_DIR)/WebServ.hpp

# SRC_FILES	=	$(SRC_DIR)/WebServ.cpp \
				$(SRC_DIR)/main.cpp

OBJ_FILES	=	$(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SRC_FILES:%.cpp=%.o))

all: $(NAME)

$(NAME): $(INC_FILES) $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DFLAGS) -I$(INC_DIR) $(OBJ_FILES) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_FILES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

test:
	@echo SRC_FILES:
	@echo $(SRC_FILES)
	@echo OBJ_FILES:
	@echo $(OBJ_FILES)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(BIN_DIR)

re: fclean all

.PHONY: all clean fclean re \
		test

