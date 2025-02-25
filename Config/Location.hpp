/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 16:52:32 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/24 15:13:30 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <sys/epoll.h>
#include <cerrno>
#include <fcntl.h>

class Server;

class Location {
    private:
        std::string URI;
        std::string root;
        std::string uploadPath;
        std::map<int, std::string> error_page;
        std::vector<std::string> allowed_methods;
        std::string index;
        bool autoindex;
        std::map<int, std::string> redirect;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_ext;
    public:
        // Constructor
        Location();
        Location(Server& copy);

        // Getters
        std::string getURI();
        std::string getRoot();
        std::string getUploadPath();
        std::map<int, std::string>& getErrorPage();
        std::string getIndex();
        bool getAutoindex();
        std::vector<std::string>& getAllowedMethods();
        std::map<int, std::string> getRedirect();
        std::vector<std::string>& getCgiPath();
        std::vector<std::string>& getCgiExt();
        
        // Setters
        void setURI(std::string& n_URI);
        void setRoot(std::string& n_root);
        void setUploadPath(std::string& name);
        void setErrorPage(std::map<int, std::string>& n_ep);
        void addErrorPage(int code, std::string path);
        void setIndex(std::string& str);
        void setAutoindex(bool n_autoindex);
        void setAllowedMethods(std::vector<std::string>& methods);
        void setRedirect(int code, std::string page);
        void addCgiPath(std::string path);
        void addCgiExt(std::string ext);
};

#endif