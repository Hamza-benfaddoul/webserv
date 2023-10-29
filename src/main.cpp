/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 11:41:00 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 18:36:47 by hbenfadd         ###   ########.fr       */
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
                
                Cluster cluster(webserv.getServerBlocks());
            }
            else
            {
                configParser webserv;
                Cluster cluster(webserv.getServerBlocks());
            }
        
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return (1);
        }

    }
    return (0);
}
