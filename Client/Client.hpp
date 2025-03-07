/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:25:29 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/07 13:59:21 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Cgi/Cgi.hpp"
#include "../Config/Server.hpp"

class Client {
    private:
        int         fdClient;
        std::string clientIP;
        Request     req;
        Response    res;
        CGI         cgi;
        Server      server;
        long long   timeout;
        char**      envp;
    public:
        //constructor
        Client();
        ~Client();
    
        // Getters
        int             getFdClient();
        std::string&    getClientIP();
        Request&        getRequest();
        Response&       getResponse();
        CGI&            getCGI();
        Server&         getServer();
        long long       getTimeout();

        // Setters
        void        setFdClient(int nfd);
        void        setServer(Server& server);
        void        setTimeout(long long t);
        void        setClientIP(std::string i);
};
