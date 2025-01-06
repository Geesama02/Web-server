/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:25:38 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/06 10:13:46 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// Getters
std::vector<Server> Config::getServer() { return Servers; }

// Setters
void Config::addServer(Server new_server) { Servers.push_back(new_server); }

// Functions
int Config::startServers() {
    epoll_event ev;
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0) {
        std::cerr << "Cannot create epoll instance!" << std::endl;
        return (1);
    }
    if (monitorServers(epoll_fd, ev))
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
        for (int i = 0; i < fds; i++) {
            if (events[i].events & EPOLLIN) { // if event is read
                int fd = events[i].data.fd;
                if (isServerFd(fd))
                    acceptConnection(fd, epoll_fd, ev);
                else
                    handleClient(fd);
            }
            else if (events[i].events & EPOLLERR) {
                std::cerr << "Socket error on fd: " << events[i].data.fd << std::endl;
                close(events[i].data.fd);
                // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            }
            else if (events[i].events & EPOLLOUT)
                std::cout << "EPOLLOUT\n";
        }
    }
    return (0);
}

int Config::monitorServers(int epoll_fd, epoll_event& ev) {
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR; // monitor if socket ready to (read | write | error)
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (it->initServer())
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

int Config::acceptConnection(int fd, int epoll_fd, epoll_event& ev) {
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
        ev.events = EPOLLIN;
        ev.data.fd = new_client;
        fcntl(new_client, F_SETFL, O_NONBLOCK);
        // add client socket to epoll to monitor
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &ev) != 0) {
            std::cerr << "epoll_ctl error: " << strerror(errno) << std::endl;
            close(new_client);
        }
    }
    return (0);
}

int Config::handleClient(int fd) {
    std::string str;
    Request request;
    std::string response =     "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 147\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "<!DOCTYPE html>"
                    "<html><body><form method=\"post\" enctype=\"multipart/form-data\">"
                        "<input type=\"file\" name=\"file\">"
                        "<button>Upload</button>"
                    "</form></body></html>";
    
    if (request.readRequest(fd))
        return (1);
    if (request.getMethod() == "GET") {
        send(fd, response.c_str(), response.size(), 0);
    } 
    return (0);
}

