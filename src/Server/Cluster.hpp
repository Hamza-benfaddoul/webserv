/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 17:49:01 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 18:39:46 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/serverBlock.hpp"
class Server;

class Cluster
{
private:
    std::vector<Server*> servers;
    Cluster();
    void run(void);
public:
    Cluster(std::vector<serverBlock> serverBlocks);
    ~Cluster();
};
