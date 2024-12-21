/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:09:59 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/21 17:43:36 by oait-laa         ###   ########.fr       */
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
};