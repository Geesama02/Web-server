/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/16 12:02:47 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define RESPONSE_HPP
#ifndef REPONSE_HPP

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
        void        fillBody();
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
        void    sendResponse(int fd);
};

#endif