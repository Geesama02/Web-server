/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:25:38 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/23 11:51:49 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "../Response/Response.hpp"
#include <sys/stat.h>

// Getters
std::vector<Server>     Config::getServers() { return Servers; }
std::map<int, Client>&  Config::getClients() { return Clients; }

// Setters
void    Config::addServer(Server new_server) { Servers.push_back(new_server); }

void    Config::checkCgiScriptExecution(int fd) {
    int status;
    std::map<int, Client>::iterator it = Clients.begin();
    while (it != Clients.end())
    {
        if (it->first == fd && Clients[fd].getCGI().getCpid() != 0) {
            pid_t child = waitpid(Clients[fd].getCGI().getCpid(), &status, WNOHANG);
            if (child > 0) {
                if (WEXITSTATUS(status))
                {
                    Clients[fd].getCGI().setChildStatus(WEXITSTATUS(status));
                    Clients[fd].getCGI().clearCGI();
                    Clients[fd].getResponse().clearResponse();
                    Clients[fd].getResponse().setStatusCode(502);
                    Clients[fd].getResponse().sendResponse(*this, Clients[fd].getRequest(), fd);
                }
                else
                { 
                    if (Clients[fd].getCGI().read_cgi_response(*this, fd) == -1) //error in cgi script
                    {
                        Clients[fd].getCGI().clearCGI();
                        Clients[fd].getResponse().clearResponse();
                        Clients[fd].getResponse().setStatusCode(502);
                        Clients[fd].getResponse().sendResponse(*this, Clients[fd].getRequest(), fd);
                        closeConnection(fd);
                        return ;
                    }
                    Clients[fd].getCGI().sendServerResponse(fd, *this);
                    Clients[fd].getCGI().clearCGI();
                }
            }
        }
        it++;
    }
}

void    Config::checkScriptTimeOut(int fd) {
    std::map<int, Client>::iterator it = Clients.begin();
    while (it != Clients.end()) {
        if (it->first == fd) {
            if (Clients[fd].getCGI().getCpid() != 0 && timeNow() - Clients[fd].getCGI().getStartTime() > 10) {
                kill(Clients[fd].getCGI().getCpid(), SIGKILL);
                waitpid(Clients[fd].getCGI().getCpid(), NULL, 0);
                close(Clients[fd].getCGI().getRpipe());
                Clients[fd].getCGI().clearCGI();
                Clients[fd].getResponse().clearResponse();
                Clients[fd].getResponse().setStatusCode(504);
                Clients[fd].getResponse().sendResponse(*this, Clients[fd].getRequest(), fd);
                closeConnection(fd);
            }
        }
        it++;
    }
}

// Functions
int Config::startServers() {
    epoll_event ev;
    epoll_fd = epoll_create(1);
    if (epoll_fd < 0) {
        std::cerr << "Cannot create epoll instance!" << std::endl;
        return (1);
    }
    if (monitorServers(ev))
        return (1);
    epoll_event events[MAX_EVENTS]; // max events(ready to read or write is an event)
    while (1) {
        // monitor if any socket ready for read or write
        int fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (fds < 0) {
            std::cerr << "Cannot wait on sockets!" << std::endl;
            close(epoll_fd);
            return (1);
        }
        for (int i = 0; i < fds; i++)
        {
            if (events[i].events & EPOLLIN) { // if event is read
                int fd = events[i].data.fd;
                if (isServerFd(fd))
                    acceptConnection(fd, ev);
                else
                    handleClient(fd);
            }
            else if (events[i].events & EPOLLERR)
            {
                std::cerr << "Socket error on fd: " << events[i].data.fd << std::endl;
                Clients.erase(events[i].data.fd);
                close(events[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
            else if (events[i].events & EPOLLOUT)
            {
                if (Clients[events[i].data.fd].getResponse().sendBodyBytes() == -1)
                {
                    Clients[events[i].data.fd].getResponse().clearResponse();
                    Clients[events[i].data.fd].getResponse().setStatusCode(500);
                    Clients[events[i].data.fd].getResponse().sendResponse(*this, Clients[events[i].data.fd].getRequest(), events[i].data.fd);
                    closeConnection(events[i].data.fd);
                }
            }
            if (events[i].data.fd != 0)
            {
                checkCgiScriptExecution(events[i].data.fd);
                checkScriptTimeOut(events[i].data.fd);
            }
        }
        monitorTimeout();
    }
    return (0);
}

int Config::monitorTimeout() {
    long long currTime = timeNow();
    int timeout = 75;
    for (std::map<int, Client>::iterator it = Clients.begin(); it != Clients.end(); ) {
        if (it->second.getTimeout() + timeout < currTime) {
            // if (Response::files.find(it->first) != Response::files.end()) {
            //     Response::files[it->first]->close();
            //     Response::files.erase(it->first);
            // }
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
            close(it->first);
            std::cout << "get KICKED\n";
            std::map<int, Client>::iterator tmp = it;
            it++;
            Clients.erase(tmp->first);
        }
        else
            it++;
    }
    return (0);
}

int Config::monitorServers(epoll_event& ev) {
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR; // monitor if socket ready to (read | write | error)
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (it->initServer(Servers,it))
            return (1);
        ev.data.fd = it->getSocket();
        // add server socket to epoll to monitor them
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, it->getSocket(), &ev) != 0) {
            std::cerr << strerror(errno) << std::endl;
            close(epoll_fd);
            return (1);
        }
    }
    return (0);
}

int Config::isServerFd(int fd) {
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (fd == it->getSocket())
            return (1);
    }
    return (0);
}

Server Config::getServer(int fd) {
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (fd == it->getSocket())
            return (*it);
    }
    return (Servers[0]);
}

long long Config::timeNow() {
    struct timeval tv;
    long long time;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    return (time);
}

int Config::acceptConnection(int fd, epoll_event& ev) {
    // loop until accepting all clients
    while (true) {
        int new_client = accept(fd, NULL, NULL);
        if (new_client < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // all clients accepted
                break;
            else {
                std::cerr << "accept error: " << strerror(errno) << std::endl;
                break;
            }
        }
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = new_client;
        fcntl(new_client, F_SETFL, O_NONBLOCK);
        Server server = getServer(fd);
        Client client;
        server.setSocket(-1);
        client.setServer(server);
        Clients[new_client] = client;
        Clients[new_client].setFdClient(new_client);
        Clients[new_client].setTimeout(timeNow());
        // std::cout << "client connection fd " <<new_client<<"!!!!"<< std::endl;
        // add client socket to epoll to monitor
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &ev) != 0) {
            std::cerr << "epoll_ctl error: " << strerror(errno) << std::endl;
            Clients.erase(new_client);
            close(new_client);
        }
    }
    return (0);
}

void Config::closeConnection(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    std::cout << "closed client : " << fd<<std::endl;
    Clients.erase(fd);
    close(fd);
}

int Config::handleClient(int fd) {
    Response res;
    int status;
    
    Clients[fd].setTimeout(timeNow());
    Clients[fd].getResponse().clearResponse();
    status = Clients[fd].getRequest().readRequest(fd, Clients[fd].getServer(), Servers);
    Clients[fd].getResponse().setStatusCode(status);
    if (status == 1) // connection is closed
        closeConnection(fd);
    else if (status == 2) // if file is uploading
        return (0);
    else {
        if (!Clients[fd].getRequest().getPath().empty())
        {
            // std::cout << "path -> " << request.getPath() << std::endl;
            std::string tmp = Clients[fd].getRequest().getHeaders()["host"];
            size_t pos = tmp.rfind(':');
            if (pos != std::string::npos)
                tmp.erase(pos);
            std::cout << Clients[fd].getServer().whichServerName(tmp) << ':' << Clients[fd].getServer().getPort()
            << " - " << Clients[fd].getRequest().getMethod() << ' ' << Clients[fd].getRequest().getPath()
            << ' ' << Clients[fd].getRequest().getVersion() << std::endl;
            if (Clients[fd].getRequest().getMethod() == "GET")
                Clients[fd].getResponse().searchForFile(Clients[fd].getRequest());
        }
        Clients[fd].getResponse().sendResponse(*this, Clients[fd].getRequest(), fd);
    }
    return (0);
}

