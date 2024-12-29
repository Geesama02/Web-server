/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/29 11:09:06 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#define MAX_EVENTS 10

class Config {
    private:
        std::vector<Server> Servers;
    public:
        // Getters
        std::vector<Server> getServer();

        // Setters
        void addServer(Server new_server);

        // Functions
        int init_sockets();
        int is_server_fd(int fd);
        int accept_connection(int fd, int epoll_fd, epoll_event& ev);
        int handle_client(int fd);
        Server get_server(int fd);
};

#endif