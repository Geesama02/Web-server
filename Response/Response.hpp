/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/17 14:56:11 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define RESPONSE_HPP
#ifndef REPONSE_HPP

#include "../Request/Request.hpp"
#include <iostream>
#include <dirent.h>
#include <map>

class Response {
    private:
        int         statusCode;
        std::string statusMssg;
        std::string body;
        std::string finalRes;
        std::map<std::string, std::string> Headers;
        std::map<std::string, std::string> ContentHeader;
        void        fillBody(Request req);
        void        initializeContentHeader();
        void        checkForFileExtension(std::string fileName);
    public:
        //constructor
        Response();

        //getters
        int         getStatusCode();
        std::string getStatusMssg();
        std::string getHeader( std::string key );

        //setters
        void    setStatusCode(int value);
        void    setStatusMssg(std::string value);
        void    setHeader( std::string key, std::string value );
        
        //other
        void    searchForFile(std::string fileName);
        void    sendResponse(int fd, Request req);
};

#endif