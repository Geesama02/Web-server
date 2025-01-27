/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/22 17:33:11 by maglagal         ###   ########.fr       */
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
        // Getters
        std::vector<Server> getServer();

        // Setters
        void addServer(Server new_server);

        // Functions
        int startServers();
        int monitorServers(int epoll_fd, epoll_event& ev);
        int isServerFd(int fd);
        int acceptConnection(int fd, int epoll_fd, epoll_event& ev);
        int handleClient(int fd, int epoll_fd);
        Server getServer(int fd);
        // int readRequest(int fd, Request& request);
        // void readHeaders(int fd, Request& request, std::string& str);
        // int setupFile(Request& request, int fd);
        // int readFile(int fd, UploadFile file, std::string str);
};

#endif