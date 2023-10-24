# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rakhsas <rakhsas@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/24 11:11:39 by rakhsas           #+#    #+#              #
#    Updated: 2023/10/24 11:18:18 by rakhsas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

CXX		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

SRCSCONFIG = configParser.cpp

#SRCSMULTIPLEX =


SRCS = $(addprefix src/, main.cpp) $(addprefix src/,$(SRCSCONFIG)) 

OBJCS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean :
	rm -f $(OBJCS)

fclean : clean
	rm -f $(NAME)

re :fclean all

run : re
	./$(NAME)
