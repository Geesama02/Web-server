/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 10:25:22 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/22 11:19:21 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Constructor
Server::Server() {
    server_name = "Server";
    host = "127.0.0.1";
    port = 8080;
    root = "/";
    error_page.push_back("404");
    error_page.push_back("/Error_pages/404.html");
    client_max_body_size = 1024;
    index = "index.html";
    autoindex = true;
    allowed_methods.push_back("POST");
    allowed_methods.push_back("GET");
    allowed_methods.push_back("DELETE");
    redirect = "";
}

// Getters
std::string Server::getServerName() { return server_name; }
std::string Server::getHost() { return host; }
int Server::getPort() { return port; }
std::string Server::getRoot() { return root; }
std::vector<std::string> Server::getErrorPage() { return error_page; }
int Server::getClientMaxBodySize() { return client_max_body_size; }
std::string Server::getIndex() { return index; }
bool Server::getAutoindex() { return autoindex; }
std::vector<std::string> Server::getAllowedMethods() { return allowed_methods; }
std::string Server::getRedirect() { return redirect; }
std::vector<Location> Server::getLocations() { return locations; }

// Setters
void Server::setServerName(std::string& name) { server_name = name; }
void Server::setHost(std::string& new_host) { host = new_host; }
void Server::setPort(int n_port) { port = n_port; }
void Server::setRoot(std::string& n_root) { root = n_root; }
void Server::setErrorPage(std::vector<std::string>& n_ep) { error_page = n_ep; }
void Server::setClientMaxBodySize(int size) { client_max_body_size = size; }
void Server::setIndex(std::string& str) { index = str; }
void Server::setAutoindex(bool n_autoindex) { autoindex = n_autoindex; }
void Server::setAllowedMethods(std::vector<std::string>& methods) { allowed_methods = methods; }
void Server::setRedirect(std::string& page) { redirect = page; }
void Server::addLocation(Location& new_location) { locations.push_back(new_location); }