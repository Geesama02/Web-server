/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:25:29 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/18 10:48:32 by oait-laa         ###   ########.fr       */
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
        long long timeout;
    public:
        // Getters
        Request& getRequest();
        Response& getResponse();
        CGI& getCGI();
        Server& getServer();
        long long getTimeout();

        // Setters
        void setServer(Server& server);
        void setTimeout(long long t);
};
