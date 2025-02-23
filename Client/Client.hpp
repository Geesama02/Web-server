/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:25:29 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/19 16:15:37 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Cgi/Cgi.hpp"
#include "../Config/Server.hpp"

class Client {
    private:
        int         fdClient;
        Request     req;
        Response    res;
        CGI         cgi;
        Server      server;
        long long   timeout;
    public:
        //constructor
        Client();
        ~Client();
    
        // Getters
        int         getFdClient();
        Request&    getRequest();
        Response&   getResponse();
        CGI&        getCGI();
        Server&     getServer();
        long long   getTimeout();

        // Setters
        void        setFdClient(int nfd);
        void        setServer(Server& server);
        void        setTimeout(long long t);
};
