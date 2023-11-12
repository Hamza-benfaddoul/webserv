# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/24 11:11:39 by rakhsas           #+#    #+#              #
#    Updated: 2023/11/11 17:21:38 by rakhsas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

CXX		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address -g3

SRCSCONFIG = configParser.cpp serverBlock.cpp Location.cpp

SRCSMULTIPLEX = Server/Server.cpp Client/Client.cpp Server/Cluster.cpp Client/Request.cpp

SRCS = $(addprefix src/, main.cpp) $(addprefix src/config/,$(SRCSCONFIG))  $(addprefix src/,$(SRCSMULTIPLEX)) helperFunction/ft_split.cpp

OBJCS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# %.o:%.cpp
# 	@printf "\033[0;33mGenerating objects... %-33.33s\r" $@
# 	@$(CC)  $(CFLAGS) -c $<  -o $@

clean :
	rm -f $(OBJCS)

fclean : clean
	rm -f $(NAME)

re :fclean all clean

run : re
	./$(NAME)
