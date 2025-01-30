/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/30 16:51:19 by maglagal         ###   ########.fr       */
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
        std::map<std::string, std::string> Headers;
        std::map<std::string, std::string> executablePaths;
        std::map<std::string, std::string> ContentHeader;
        int                                statusCode;
        std::string                        statusMssg;
        std::string                        body;
        std::string                        finalRes;
        void                               fillBody(Request req, int fd);
        void                               initializeContentHeader();
        void                               checkForFileExtension(Request req, std::string fileName);
    public:
        static std::map<int, std::ifstream *> files;
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
        void            successResponse(Request req, int fd);
        void            notFoundResponse();
        void            forbiddenResponse();
        void            searchForFile(Request Req);
        void            sendResponse(int fd, Request req, char **envp);
        static void     sendBodyBytes(int fd);
        void            handleRangeRequest(Request req, int fd);
        void            initializeExecutablePaths();
        void            execute_cgi_script(int fd, Request req, char **envp);
};

#endif