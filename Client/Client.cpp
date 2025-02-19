/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:34:46 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/18 10:50:05 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// #include "../Cgi/Cgi.hpp"

// Getters
Request& Client::getRequest() { return (req); }
Response& Client::getResponse() { return (res); }
CGI& Client::getCGI() { return (cgi); }
Server& Client::getServer() { return (server); }
long long Client::getTimeout() { return (timeout); }

// Setters
void Client::setServer(Server& s) { server = s; }
void Client::setTimeout(long long t) { timeout = t; }