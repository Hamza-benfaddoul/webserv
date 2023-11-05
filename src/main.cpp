/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 11:41:00 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/30 10:02:54 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"

int main(int ac, char **av)
{
    if (ac <= 2)
    {
        try
        {
            if (av[1])
            {
                configParser webserv(av[1]);
                
                //   Cluster cluster(webserv.getServerBlocks());
            }
            else
            {
                configParser webserv;
                // Cluster cluster(webserv.getServerBlocks());
            }
        
        }
        catch (const std::exception &e)
        {
            std::cout << "\e[0;31m" << e.what() << "\033[0m"<< std::endl;
            return (1);
        }

    }
    return (0);
}
