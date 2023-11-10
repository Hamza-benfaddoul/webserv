/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/28 15:08:29 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "Request.hpp"

class Client {
	private:
		Client();
		std::string _responseBuffer;
		size_t		_fd;
		fd_set		&_readfds;
		Request	*request;
		// Response	*response;
		void	getMethodHandler(void);
		void	postMethodHandler(void);
		void	receiveResponse(void);
		void	sendResponse(void);
		void	closeConnection(void);

	public:
		Client(size_t fd, fd_set &readfds);
		~Client();
		void	run(void);
};