/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:09:59 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/22 10:49:42 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

class Server {
    private:
        std::string server_name;
        std::string host;
        int port;
        std::string root;
        std::vector<std::string> error_page;
        int client_max_body_size;
        std::string index;
        bool autoindex;
        std::vector<std::string> allowed_methods;
        std::string redirect;
        std::vector<Location> locations;
    public:
        // Contructor
        Server();

        // Getters
        std::string getServerName();
        std::string getHost();
        int getPort();
        std::string getRoot();
        std::vector<std::string> getErrorPage();
        int getClientMaxBodySize();
        std::string getIndex();
        bool getAutoindex();
        std::vector<std::string> getAllowedMethods();
        std::string getRedirect();
        std::vector<Location> getLocations();

        // Setters
        void setServerName(std::string& name);
        void setHost(std::string& new_host);
        void setPort(int n_port);
        void setRoot(std::string& n_root);
        void setErrorPage(std::vector<std::string>& n_ep);
        void setClientMaxBodySize(int size);
        void setIndex(std::string& str);
        void setAutoindex(bool n_autoindex);
        void setAllowedMethods(std::vector<std::string>& methods);
        void setRedirect(std::string& page);
        void addLocation(Location& new_location);
};