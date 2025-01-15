/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/15 15:31:24 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define RESPONSE_HPP
#ifndef REPONSE_HPP

#include <iostream>
#include <dirent.h>

class Response {
    private:
        std::string connection;
        int         contentLength;
        std::string eTag;
        std::string lastModified;
        std::string server;
    public:
        //getters
        std::string getConnection();
        int         getContentLength();
        std::string getEtag();
        std::string getLastModified();
        std::string getServer();

        //setters
        void setConnection(std::string value);
        void setContentLength(int value);
        void setEtag(std::string value);
        void setLastModified(std::string value);
        void setServer(std::string value);
        
        //other
        int searchForFile(std::string fileName);

};

#endif