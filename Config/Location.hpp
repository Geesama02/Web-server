/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 16:52:32 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/27 16:46:19 by oait-laa         ###   ########.fr       */
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
#include <string>
#include <cstdio>
#include <sys/epoll.h>
#include <cerrno>

class Location {
    private:
        std::string URI;
        std::string root;
        std::map<std::vector<int>, std::string> error_page;
        std::vector<std::string> allowed_methods;
        std::string index;
        bool autoindex;
        std::string redirect;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_ext;
    public:
        // Constructor
        Location();

        // Getters
        std::string getURI();
        std::string getRoot();
        std::map<std::vector<int>, std::string>& getErrorPage();
        std::string getIndex();
        bool getAutoindex();
        std::vector<std::string>& getAllowedMethods();
        std::string getRedirect();
        std::vector<std::string>& getCgiPath();
        std::vector<std::string>& getCgiExt();
        
        // Setters
        void setURI(std::string& name);
        void setRoot(std::string& name);
        void setErrorPage(std::map<std::vector<int>, std::string>& n_ep);
        void setIndex(std::string& str);
        void setAutoindex(bool n_autoindex);
        void setAllowedMethods(std::vector<std::string>& methods);
        void setRedirect(std::string& page);
        void addCgiPath(std::string path);
        void addCgiExt(std::string ext);
};

#endif