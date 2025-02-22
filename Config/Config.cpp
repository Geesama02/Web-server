/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:25:38 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/22 16:43:38 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "../Response/Response.hpp"
#include <sys/stat.h>

std::map<int, Response> Config::Responses;

// Getters
std::map<int, CGI>&     Config::getCgiScripts() { return cgiScriptContainer; }
std::vector<Server>     Config::getServers() { return Servers; }
bool                    Config::getTimeoutResponseFlag() { return timeoutResponseFlag; }
std::map<int, Client>& Config::getClients() { return Clients; }

// Setters
void    Config::setCgiScripts(int fd, const CGI& newCgiScript) { 
    // std::cout << "main child created " << newCgiScript.getCpid()<<std::endl;
    cgiScriptContainer[fd] = newCgiScript;
    // std::cout << "here !!" <<std::endl;
}
void    Config::addServer(Server new_server) { Servers.push_back(new_server); }
void    Config::setTimeoutResponseFlag(bool nValue) { timeoutResponseFlag = nValue; }


void    timeoutResponse(int fd) {
    std::string res;
    std::string statusMssg = "408 Request Timeout\r\n";
    res += statusMssg;
    std::string body = "<!DOCTYPE html>"
                "<html><head><title>408 Request Timeout</title>"
                "<style>h1, p {text-align:center}</style></head><body>"
                "<h1>408 Request Timeout</h1>"
                "<hr></hr>"
                "<p>Webserv</p>"
                "</body></html>";
    char buff[150];
    sprintf(buff, "%ld", body.length());
    std::string contentLengthHeader = buff;
    contentLengthHeader = "Content-Length :" + contentLengthHeader;
    std::string contentTypeHeader = "text/html";
    contentTypeHeader = "Content-Type :" + contentTypeHeader;
    std::string headers = contentLengthHeader + "\r\n" + contentTypeHeader;
    headers += "\r\n";
    res += headers;
    res += body;
    // std::cout << "timeout response" << res<<std::endl;
    send(fd, res.c_str(), res.length(), 0);
}

void    Config::checkCgiScriptExecution(int fd) {
    std::map<int, CGI>::iterator it = cgiScriptContainer.begin();
    while (it != cgiScriptContainer.end()) 
    {
        if (it->first == fd && (it->second).getCpid() != 0) {
            pid_t child = waitpid(cgiScriptContainer[fd].getCpid(), NULL, WNOHANG);
            // std::cout << "child pid from waitpid "<<child<<std::endl;
            if (child == 0) {
                // std::cout << "client " << fd<<std::endl;
                // std::cout << "child pid "<<cgiScriptContainer[fd].getCpid()<<std::endl;
                // std::cout << "child not yet finished exeuction!!"<<std::endl;
            }
            else if(child == -1) {
                // std::cout << "child pid "<<cgiScriptContainer[fd].getCpid()<<std::endl;
                // std::cerr<< "error in child!!" << std::endl;
                std::cout <<strerror(errno)<<std::endl;
            }
            else {
                // std::cout << "child pid "<<cgiScriptContainer[fd].getCpid()<<std::endl;
                // std::cout << "child finished exeuction!!"<<std::endl;
            
                // read the ouput of the script executed by the cgi
                // std::cout <<"send response!!"<<std::endl;
                cgiScriptContainer[fd].read_cgi_response();
                // close(fds[0]);
                // std::cout << "res body "<< Responses[fd].body<<std::endl;
                cgiScriptContainer[fd].sendServerResponse(fd, Responses[fd]);
                cgiScriptContainer.clear();
            }
        }
        it++;
    }
}

void    Config::checkScriptTimeOut(int fd) {
    std::map<int, CGI>::iterator it = cgiScriptContainer.begin();
    while (it != cgiScriptContainer.end()) 
    {
        if (it->first == fd) {
            if (cgiScriptContainer[fd].getCpid() != 0 && timeNow() - cgiScriptContainer[fd].getStartTime() > 5) {
                // std::cout << "client fd "<<fd<<std::endl;
                // std::cout << "timeout "<<timeNow() - cgiScriptContainer[fd].getStartTime()<<std::endl;
                // std::cout << "child timeout!!! " << cgiScriptContainer[fd].getCpid()<<std::endl;
                kill(cgiScriptContainer[fd].getCpid(), SIGKILL);
                close(cgiScriptContainer[fd].getRpipe());
                timeoutResponseFlag = true;
                timeoutResponse(fd);
                cgiScriptContainer.clear();
            }
        }
        it++;
    }   
}

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
                else {
                    // std::cout << "request sent!! " <<"client "<<events[i].data.fd<<std::endl;
                    handleClient(fd, epoll_fd);
                }
            }
            else if (events[i].events & EPOLLERR) {
                std::cerr << "Socket error on fd: " << events[i].data.fd << std::endl;
                Clients.erase(events[i].data.fd);
                close(events[i].data.fd);
                // if (clientTimeout.find(events[i].data.fd) != clientTimeout.end())
                //     clientTimeout.erase(events[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                if (Response::files.find(events[i].data.fd) != Response::files.end()) {
                    Response::files[events[i].data.fd]->close();
                    Response::files.erase(events[i].data.fd);
                }
            }
            else if (events[i].events & EPOLLOUT)
                Responses[events[i].data.fd].sendBodyBytes();
            if (!getCgiScripts().empty() && events[i].data.fd != 0) {
                checkCgiScriptExecution(events[i].data.fd);
                checkScriptTimeOut(events[i].data.fd);
            }
        }
        monitorTimeout(epoll_fd);
    }
    return (0);
}

// void    Config::monitorTimeoutCgiScripts(int cFd) {
//     std::map<int, Child>::iterator it = CGI::childsExecuting.begin();
//     while (it != CGI::childsExecuting.end()) {
//         if (timeNow() - CGI::childsExecuting[cFd].getStartTime() 
//             > CGI::childsExecuting[cFd].getTimeout())
//         {
//             kill((it->second).getCpid(), SIGKILL);
//             close((it->second).getRpipe());
//         }
//     }
// }

int Config::monitorTimeout(int epoll_fd) {
    long long currTime = timeNow();
    int timeout = 75;
    for (std::map<int, Client>::iterator it = Clients.begin(); it != Clients.end(); ) {
        if (it->second.getTimeout() + timeout < currTime) {
            if (Response::files.find(it->first) != Response::files.end()) {
                Response::files[it->first]->close();
                Response::files.erase(it->first);
            }
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

int Config::monitorServers(int epoll_fd, epoll_event& ev) {
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
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = new_client;
        fcntl(new_client, F_SETFL, O_NONBLOCK);
        Server server = getServer(fd);
        Client client;
        server.setSocket(-1);
        client.setServer(server);
        Clients[new_client] = client;
        Clients[new_client].setTimeout(timeNow());
        // add client socket to epoll to monitor
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &ev) != 0) {
            std::cerr << "epoll_ctl error: " << strerror(errno) << std::endl;
            Clients.erase(new_client);
            close(new_client);
        }
    }
    return (0);
}

void Config::closeConnection(int epoll_fd, int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    std::cout << "closed\n";
    Clients.erase(fd);
    if (Response::files.find(fd) != Response::files.end()) {
        Response::files[fd]->close();
        Response::files.erase(fd);
    }
    close(fd);
}

int Config::handleClient(int fd, int epoll_fd) {
    Response res;
    int status;
    
    Clients[fd].setTimeout(timeNow());
    status = Clients[fd].getRequest().readRequest(fd, Clients[fd].getServer(), Servers);
    std::cout << "status -> " << status << std::endl;
    if (status == 1) // connection is closed
        closeConnection(epoll_fd, fd);
    else if (status == 2) // if file is uploading
        return (0);
    else if (status != 0) {
        std::string res = Clients[fd].getRequest().generateRes(status);
        // std::cout << res << std::endl;
        send(fd, res.c_str(), res.size(), 0);
        if (status >= 400)
            closeConnection(epoll_fd, fd);
        return (0);
    }
    else {
        if (!Clients[fd].getRequest().getPath().empty()) {
            // std::cout << "path -> " << Clients[fd].getRequest().getPath() << std::endl;
            // for (std::map<std::string, std::string>::iterator it = Clients[fd].getRequest().getHeaders().begin() ; it != Clients[fd].getRequest().getHeaders().end(); it++) {
            //     std::cout << it->first << " = " << it->second << std::endl;
            // }
            std::string tmp = Clients[fd].getRequest().getHeaders()["host"];
            size_t pos = tmp.rfind(':');
            if (pos != std::string::npos)
                tmp.erase(pos);
            std::cout << Clients[fd].getServer().whichServerName(tmp) << ':' << Clients[fd].getServer().getPort()
            << " - " << Clients[fd].getRequest().getMethod() << ' ' << Clients[fd].getRequest().getPath()
            << ' ' << Clients[fd].getRequest().getVersion() << std::endl;
            res.searchForFile(Clients[fd].getRequest());
        }
        res.sendResponse(*this, Clients[fd].getRequest(), fd);
    }
    return (0);
}

