/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/09 17:09:02 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <sys/stat.h>
#include <arpa/inet.h>

#include "../Client/Client.hpp"
// #include "../Request/Request.hpp"
#define MAX_EVENTS 1024

class Request;
class Response;

class Config {
    private:
        int                   epoll_fd;
        std::vector<Server>   Servers;
        std::map<int, Client> Clients;
    public:
        // Getters
        std::vector<Server>    getServers();
        std::map<int, Client>& getClients();

        // Setters
        void addServer(Server new_server);

        // Functions
        void    checkCgiScriptExecution();
        void    checkScriptTimeOut();
        int     startServers();
        int     monitorServers(epoll_event& ev);
        int     isServerFd(int fd);
        int     acceptConnection(int fd, epoll_event& ev);
        int     handleClient(int fd);
        Server  getServer(int fd);
        static  long long timeNow();
        int     monitorTimeout();
        void    closeConnection(int fd);
        void    printLog(int fd);
        void    normalizePath(Request& req);
        void    checkFileSend(int fd);
};

#endif
