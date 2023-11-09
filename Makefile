# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/24 11:11:39 by rakhsas           #+#    #+#              #
#    Updated: 2023/10/29 18:31:38 by hbenfadd         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

SRCSCONFIG = configParser.cpp serverBlock.cpp Location.cpp

SRCSMULTIPLEX = Server/Server.cpp Client/Client.cpp Server/Cluster.cpp Client/Request.cpp

SRCS = $(addprefix src/, main.cpp) $(addprefix src/config/,$(SRCSCONFIG))  $(addprefix src/,$(SRCSMULTIPLEX)) helperFunction/ft_split.cpp

OBJCS = $(SRCS:.cpp=.o)

all : $(NAME) clean

$(NAME) : $(OBJCS)
	$(CC) $(CFLAGS) $^ -o $@

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
