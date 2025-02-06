/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/05 16:16:47 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <dirent.h>
#include <map>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <string>
#include <sstream>

#include "../Request/Request.hpp"
#include "../cgi/cgi.hpp"

class Response {
    private:
        std::map<std::string, std::string> Headers;
        int                                statusCode;
        std::string                        statusMssg;
        std::string                        body;
        std::string                        finalRes;
        void                               fillBody(Request req, int fd);
        void                               initializeContentHeader();
        void                               checkForFileExtension(std::string fileName);
    public:
        static std::map<std::string, std::string> ContentHeader;
        static std::map<int, std::ifstream *> files;
        //constructor
        Response();

        //getters
        int         getStatusCode();
        std::string getStatusMssg();
        std::map<std::string, std::string>& getHeadersRes( );
        std::string getHeader( std::string key );

        //setters
        void    setStatusCode(int value);
        void    setStatusMssg(std::string value);
        void    setHeader( std::string key, std::string value );
        
        //other
        void            successResponse(Request req, int fd);
        void            notFoundResponse();
        void            forbiddenResponse();
        void            searchForFile(Request Req);
        void            sendResponse(int fd, Request req);
        static void     sendBodyBytes(int fd);
        void            handleRangeRequest(Request req, int fd);
};

#endif