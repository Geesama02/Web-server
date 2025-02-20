/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/20 09:30:34 by maglagal         ###   ########.fr       */
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
#include <fstream>

// #include "../Request/Request.hpp"
// #include "../Config/Config.hpp"
#include "../Cgi/Cgi.hpp"

class Config;
class Location;
class Request;

class Response {
    private:
        std::ifstream *file;
        std::map<std::string, std::string> Headers;
        int                                clientFd;
        int                                statusCode;
        std::string                        statusMssg;
        std::string                        finalRes;
        std::string                        queryString;
        std::string                        currentDirAbsolutePath;
        void                               fillBody(Config& config, Request req);
        void                               initializeContentHeader();
        void                               checkForFileExtension(std::string fileName);
        void                               matchReqPathWithLocation(Config& config, Location loc, std::string reqPath, std::string toMatch);
        void                               returnDefinedPage(std::string rootPath, std::string errorPageFile);
        int                                checkDefinedErrorPage(std::string rootPath, std::map<int, std::string> error_page);
        void                               checkAutoIndex(Config& config, Request req);
        void                               listDirectories(std::string dirName);
        void                               showIndexFile(std::string indexFilePath);
        int                                comparingReqWithLocation(std::string locationPath, std::string reqPath);
        void                               vertifyDirectorySlash(std::string fileName);
        void                               fromIntTochar(int number, char **buff);
    public:
        // std::map<int, std::ifstream *>     files;
        // size_t                                    totalBytesSent;
        // size_t                                    bytesToSend;
        std::string                               body;
        static std::map<std::string, std::string> ContentHeader;

        //constructor
        Response();
        ~Response();

        //getters
        int                                 getClientFd();
        int                                 getStatusCode();
        std::string                         getQueryString();
        std::string                         getStatusMssg();
        std::map<std::string, std::string>& getHeadersRes( );
        std::string                         getHeader( std::string key );

        //setters
        void            setClientFd(int nFd);
        void            setStatusCode(int value);
        void            setQueryString(std::string value);
        void            setStatusMssg(std::string value);
        void            setHeader( std::string key, std::string value );
        
        //other
        void            clearResponse();
        void            timeoutResponse(int fd);
        void            checkForQueryString(std::string& fileName);
        void            successResponse(Request req);
        void            notFoundResponse();
        void            forbiddenResponse();
        void            redirectionResponse(Request req, Config& config);
        void            searchForFile(Request Req);
        void            sendResponse(Config& config, Request req, int fd);
        void            sendBodyBytes();
        void            handleRangeRequest(Request req);
};

#endif