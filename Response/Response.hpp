/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/10 15:37:12 by maglagal         ###   ########.fr       */
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
#include <sys/types.h>

#include "../Request/Request.hpp"
#include "../cgi/cgi.hpp"

class Response {
    private:
        std::map<std::string, std::string> Headers;
        int                                clientFd;
        int                                statusCode;
        std::string                        statusMssg;
        std::string                        body;
        std::string                        finalRes;
        std::string                        queryString;
        std::string                        currentDirAbsolutePath;
        void                               fillBody(Config& config, Request req);
        void                               initializeContentHeader();
        void                               checkForFileExtension(std::string fileName);
        void                               matchReqPathWithLocation(Location loc, std::string reqPath);
        void                               returnDefinedPage(std::string fileName);
        void                               checkDefinedPage(Config& config, Request req);
        void                               checkAutoIndex(Config& config, Request req);
        void                               listDirectories(std::string locationPath, std::string dirName);
        void                               showIndexFile(std::string indexFilePath);
    public:
        static std::map<std::string, std::string> ContentHeader;
        static std::map<int, std::ifstream *> files;
        static std::map<int, Response> Responses;

        //constructor
        Response();
        ~Response();

        //getters
        int         getStatusCode();
        std::string getQueryString();
        std::string getStatusMssg();
        std::map<std::string, std::string>& getHeadersRes( );
        std::string getHeader( std::string key );

        //setters
        void    setStatusCode(int value);
        void    setQueryString(std::string value);
        void    setStatusMssg(std::string value);
        void    setHeader( std::string key, std::string value );
        
        //other
        void            checkForQueryString(std::string& fileName);
        void            successResponse(Request req);
        void            notFoundResponse();
        void            forbiddenResponse();
        void            searchForFile(Request Req);
        void            sendResponse(Config& config, Request req, int fd);
        void            sendBodyBytes();
        void            handleRangeRequest(Request req);
};

#endif