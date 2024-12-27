/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:09:59 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/27 12:20:12 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"

class Server {
    private:
        int socket_fd;
        std::string server_name;
        std::string host;
        int port;
        std::string root;
        std::map<std::vector<int>, std::string> error_page;
        size_t client_max_body_size;
        std::string index;
        std::string redirect;
        std::vector<Location> locations;
    public:
        // Contructor
        Server();

        // Getters
        int getSocket();
        std::string getServerName();
        std::string getHost();
        int getPort();
        std::string getRoot();
        std::map<std::vector<int>, std::string>& getErrorPage();
        size_t getClientMaxBodySize();
        std::string getIndex();
        std::string getRedirect();
        std::vector<Location> getLocations();

        // Setters
        void setServerName(std::string& name);
        void setHost(std::string& new_host);
        void setPort(int n_port);
        void setRoot(std::string& n_root);
        void setErrorPage(std::map<std::vector<int>, std::string>& n_ep);
        void setClientMaxBodySize(size_t size);
        void setIndex(std::string& str);
        void setRedirect(std::string& page);
        void addLocation(Location& new_location);

        // Functions
        int init_server();

        // Destructor
        ~Server();
};

#endif