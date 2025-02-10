/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/09 11:23:22 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <sys/stat.h>
#include <arpa/inet.h>
#include "../Request/Request.hpp"
#define MAX_EVENTS 10

class Config {
    private:
        std::vector<Server> Servers;
        std::map<int, Server> clientServer;
    public:
        static std::map<int, long long> clientTimeout;
        
        // Getters
        std::vector<Server> getServers();
        std::map<int, Server>& getClientServer();

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
};

#endif