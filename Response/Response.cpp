/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/15 15:31:15 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>

//getters
std::string Response::getConnection() { return connection; };
int         Response::getContentLength() { return contentLength; };
std::string Response::getEtag() { return eTag; };
std::string Response::getLastModified() { return lastModified; };
std::string Response::getServer() { return server; };

//setters
void Response::setConnection(std::string value) { connection = value; };
void Response::setContentLength(int value) { contentLength = value; }; 
void Response::setEtag(std::string value) { eTag = value; };
void Response::setLastModified(std::string value) { lastModified = value; };
void Response::setServer(std::string value) { server =  value; };

//other
int Response::searchForFile(std::string fileName) {
    struct stat st;

    if (fileName != "/")
        fileName.erase(0, 1);
    else
        return (0);
    std::cout << fileName << std::endl;
    if (!stat(fileName.c_str(), &st)) {
        std::cout << "FOUND!!\n";
        if (st.st_mode & S_IFDIR) {
            std::cout << "DIR!!!\n";
            return (403);
        }
        else if (st.st_mode & S_IFREG) {
            std::cout << "file!!!\n";
            return (200);
        }
    }
    return (404);
}
