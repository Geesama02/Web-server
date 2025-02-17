/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/17 16:35:52 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <sys/stat.h>
#include <arpa/inet.h>
#include "../Client/Client.hpp"
// #include "../Request/Request.hpp"
#define MAX_EVENTS 10

class Config {
    private:
        std::vector<Server> Servers;
        // std::map<int, Server> clientServer;
        std::map<int, Client> Clients;
    public:
        static std::map<int, long long> clientTimeout;
        
        // Getters
        std::vector<Server> getServers();
        std::map<int, Client>& getClients();

        // Setters
        void addServer(Server new_server);

        // Functions
        int startServers();
        int monitorServers(int epoll_fd, epoll_event& ev);
        int isServerFd(int fd);
        int acceptConnection(int fd, int epoll_fd, epoll_event& ev);
        int handleClient(int fd, int epoll_fd);
        Server getServer(int fd);
        static long long timeNow();
        int monitorTimeout(int epoll_fd);
        void closeConnection(int epoll_fd, int fd);
};

#endif