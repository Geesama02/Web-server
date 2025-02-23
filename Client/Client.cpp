/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:34:46 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/20 09:42:56 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// #include "../Cgi/Cgi.hpp"

Client::Client() {}
Client::~Client() {}

// Getters
int      Client::getFdClient() { return fdClient; }
Request& Client::getRequest() { return (req); }
Response& Client::getResponse() { return (res); }
CGI& Client::getCGI() { return (cgi); }
Server& Client::getServer() { return (server); }
long long Client::getTimeout() { return (timeout); }

// Setters
void Client::setFdClient(int nFd) { fdClient = nFd; }
void Client::setServer(Server& s) { server = s; }
void Client::setTimeout(long long t) { timeout = t; }