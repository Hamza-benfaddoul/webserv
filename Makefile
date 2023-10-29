# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rakhsas <rakhsas@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/24 11:11:39 by rakhsas           #+#    #+#              #
#    Updated: 2023/10/28 13:06:32 by rakhsas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv.exe

CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 # -fsanitize=address -g3

SRCSCONFIG = configParser.cpp serverBlock.cpp

#SRCSMULTIPLEX =

# SRCS = src/main.cpp  src/configParser.cpp
SRCS = $(addprefix src/, main.cpp) $(addprefix src/config/,$(SRCSCONFIG)) 

OBJCS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJCS)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.cpp
	@printf "\033[0;33mGenerating objects... %-33.33s\r" $@
	@$(CC)  $(CFLAGS) -c $<  -o $@

clean :
	rm -f $(OBJCS)

fclean : clean
	rm -f $(NAME)

re :fclean all

run : re
	./$(NAME)
