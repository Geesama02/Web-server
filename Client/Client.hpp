/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:25:29 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/17 14:13:09 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Cgi/Cgi.hpp"
#include "../Config/Server.hpp"

class Client {
    private:
        Request req;
        Response res;
        CGI cgi;
        Server server;
    public:
        // Getters
        Request& getRequest();
        Response& getResponse();
        CGI& getCGI();
        Server& getServer();

        // Setters
        void setServer(Server& server);
};
