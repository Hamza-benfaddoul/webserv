/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 17:49:01 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/12 23:12:43 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/serverBlock.hpp"
#include "Server.hpp"
#include "../Client/Client.hpp"
class Server;

class Cluster
{
private:
    std::vector<Server*> _servers;
    std::vector<Client*> _clients;
    Cluster();
    void run(void);
public:
    Cluster(std::vector<serverBlock> serverBlocks);
    ~Cluster();
};
