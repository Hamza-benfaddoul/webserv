/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 17:58:17 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 18:40:56by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/main.hpp"
#include <arpa/inet.h>


Cluster::Cluster() {}

Cluster::Cluster( std::vector<serverBlock> serverBlocks)
{
    // create a servers
    for (std::vector<serverBlock>::iterator it = serverBlocks.begin() + 1; it != serverBlocks.end(); ++it)
    {
        servers.push_back(new Server(it->getHost(), it->getPort()));
    }
    // run all servers
    run();
}

Cluster::~Cluster()
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        delete servers[i];
    }
}

void Cluster::run(void)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (fork())
            servers[i]->run();
    }
}