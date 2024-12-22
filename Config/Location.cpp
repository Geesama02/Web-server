/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:17:32 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/22 11:22:09 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

// Constructor
Location::Location() {
    URI = "/";
    root = "/";
    error_page.push_back("404");
    error_page.push_back("/Error_pages/404.html");
    index = "index.html";
    autoindex = true;
    allowed_methods.push_back("POST");
    allowed_methods.push_back("GET");
    allowed_methods.push_back("DELETE");
    redirect = "";
}

// Getters
std::string Location::getURI() { return URI; }
std::string Location::getRoot() { return root; }
std::vector<std::string> Location::getErrorPage() { return error_page; }
std::string Location::getIndex() { return index; }
bool Location::getAutoindex() { return autoindex; }
std::vector<std::string> Location::getAllowedMethods() { return allowed_methods; }
std::string Location::getRedirect() { return redirect; }

// Setters
void Location::setURI(std::string& n_URI) { root = n_URI; }
void Location::setRoot(std::string& n_root) { root = n_root; }
void Location::setErrorPage(std::vector<std::string>& n_ep) { error_page = n_ep; }
void Location::setIndex(std::string& str) { index = str; }
void Location::setAutoindex(bool n_autoindex) { autoindex = n_autoindex; }
void Location::setAllowedMethods(std::vector<std::string>& methods) { allowed_methods = methods; }
void Location::setRedirect(std::string& page) { redirect = page; }