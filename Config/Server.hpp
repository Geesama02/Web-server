/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:09:59 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/07 17:38:56 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"

class Server {
    private:
        int socket_fd;
        std::vector<std::string> server_name;
        std::string host;
        int port;
        std::string root;
        bool autoindex;
        std::map<int, std::string> error_page;
        long long client_max_body_size;
        std::string index;
        std::map<int, std::string> redirect;
        std::string cgi_dir;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_ext;
        std::vector<Location> locations;
    public:
        // Contructor
        Server();

        // Getters
        int getSocket();
        std::vector<std::string>& getServerName();
        std::string whichServerName(std::string str);
        std::string getHost();
        int getPort();
        std::string getRoot();
        std::map<int, std::string>& getErrorPage();
        long long getClientMaxBodySize();
        std::string getIndex();
        bool getAutoindex();
        std::map<int, std::string>& getRedirect();
        std::string& getCgiDir();
        std::vector<std::string>& getCgiPath();
        std::vector<std::string>& getCgiExt();
        std::vector<Location>& getLocations();

        // Setters
        void setSocket(int s);
        void setServerName(std::string& name);
        void setHost(std::string& new_host);
        void setPort(int n_port);
        void setRoot(std::string& n_root);
        void setAutoindex(bool n_autoindex);
        void setErrorPage(std::map<int, std::string>& n_ep);
        void addErrorPage(int code, std::string path);
        void setClientMaxBodySize(long long size);
        void setIndex(std::string& str);
        void setRedirect(int code, std::string page);
        void setCgiDir(std::string dir);
        void addCgiPath(std::string path);
        void addCgiExt(std::string ext);
        void addLocation(Location& new_location);

        // Functions
        int initServer(std::vector<Server>& Servers, std::vector<Server>::iterator it);
        int checkPortDup(std::vector<Server>& Servers, std::vector<Server>::iterator it);
        int checkNameDup(std::vector<Server>& Servers, std::vector<Server>::iterator it, std::string& s_name);

        // Destructor
        ~Server();
};

#endif