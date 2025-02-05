/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/01 14:03:36 by oait-laa         ###   ########.fr       */
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
        int         statusCode;
        std::string statusMssg;
        std::string body;
        std::string finalRes;
        void        fillBody(Request req, int fd);
        void        initializeContentHeader();
        void        checkForFileExtension(std::string fileName);
    public:
        static std::map<std::string, std::string> ContentHeader;
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
        void            sendResponse(int fd, Request req);
        static void     sendBodyBytes(int fd);
        void            handleRangeRequest(Request req, int fd);
};

#endif