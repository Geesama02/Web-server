/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:34:46 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/06 13:02:42 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// #include "../Cgi/Cgi.hpp"

// Getters
int      Client::getFdClient() { return fdClient; }
std::string&    Client::getClientIP() { return clientIP; }
Request& Client::getRequest() { return (req); }
Response& Client::getResponse() { return (res); }
CGI& Client::getCGI() { return (cgi); }
Server& Client::getServer() { return (server); }
long long Client::getTimeout() { return (timeout); }

// Setters
void Client::setFdClient(int nFd) { fdClient = nFd; }
void Client::setServer(Server& s) { server = s; }
void Client::setTimeout(long long t) { timeout = t; }
void Client::setClientIP(std::string i) { clientIP = i; }
