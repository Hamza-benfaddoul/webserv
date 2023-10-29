/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 11:41:00 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 17:44:33 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"
#include "./Server/Server.hpp"

int main(int ac, char **av)
{
    if (ac > 0)
    {
        try
        {
            if (av[1])
                configParser webserv(av[1]);
            else
                configParser webserv;
        }
        catch (const std::exception &e)
        {
            std::cout << "\e[0;31m" << e.what() << "\033[0m" << std::endl;
        }

        Server S1(INADDR_ANY, 8080);
        try
        {
            S1.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return (1);
        }

    }
    else
        std::cout << "Usage: ./webserv [config_file]" << std::endl;
    return (0);
}
