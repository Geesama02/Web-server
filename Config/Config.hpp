/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/19 10:30:52 by maglagal         ###   ########.fr       */
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

class Request;
class Response;

class Config {
    private:
        std::map<int, CGI>    cgiScriptContainer;
        bool                  timeoutResponseFlag;       
        std::vector<Server> Servers;
        std::map<int, Client> Clients;
    public:
        static std::map<int, Response>  Responses;
        
        // Getters
        std::map<int, CGI>&    getCgiScripts();
        bool                   getTimeoutResponseFlag();
        std::vector<Server> getServers();
        std::map<int, Client>& getClients();

        // Setters
        void setCgiScripts(int fd, const CGI& newCgiScript);
        void addServer(Server new_server);
        // void setResponseReady(bool nValue);
        void setTimeoutResponseFlag(bool nValue);

        // Functions
        void    checkCgiScriptExecution(int fd);
        void    checkScriptTimeOut(int fd);
        int     startServers();
        int     monitorServers(int epoll_fd, epoll_event& ev);
        int     isServerFd(int fd);
        int     acceptConnection(int fd, int epoll_fd, epoll_event& ev);
        int     handleClient(int fd, int epoll_fd);
        Server  getServer(int fd);
        static  long long timeNow();
        int     monitorTimeout(int epoll_fd);
        void    closeConnection(int epoll_fd, int fd);
};

#endif