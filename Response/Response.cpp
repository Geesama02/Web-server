/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/17 15:59:13 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <limits>

//constructor
Response::Response() {
    initializeContentHeader();
    Headers["Content-Type"] = "text/html";
    Headers["Connection"] = "keep-alive";
    Headers["Server"] = "Webserv";
    Headers["Content-Length"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
}

//getters
int         Response::getStatusCode() { return statusCode; };
std::string Response::getStatusMssg() { return statusMssg; };
std::string Response::getHeader( std::string key ) { return (Headers.find(key)->second); };

//setters
void Response::setStatusCode(int value) { statusCode = value; };
void Response::setStatusMssg( std::string value ) { statusMssg = value; };
void Response::setHeader( std::string key, std::string value ) { Headers.find(key)->second = value; };

//other
void Response::initializeContentHeader() {
    ContentHeader[".txt"] = "text/plain";
    ContentHeader[".html"] = "text/html";
    ContentHeader[".css"] = "text/css";
    ContentHeader[".js"] = "application/javascript";
    ContentHeader[".pdf"] = "application/pdf";
    ContentHeader[".png"] = "image/png";
    ContentHeader[".jpg"] = "image/jpeg";
    ContentHeader[".jpeg"] = "image/jpeg";
    ContentHeader[".mp4"] = "video/mp4";
}

void Response::checkForFileExtension(std::string extension) {
    size_t pos = extension.rfind(".");
    if (pos != std::string::npos) {
        extension.erase(0, pos);
        std::map<std::string, std::string>::iterator it = ContentHeader.begin();
        while(it != ContentHeader.end()) {
            if (it->first == extension) {
                setHeader("Content-Type", it->second);
                return ;
            }
            it++;
        }
    }
    setHeader("Content-Type", "application/stream-octet");
}

void Response::searchForFile(std::string fileName) {
    struct stat st;

    if (fileName != "/")
        fileName.erase(0, 1);
    else {
        statusCode = 200;
        setHeader("Content-Type", "text/html");
        return ;
    }
    if (!stat(fileName.c_str(), &st)) {
        if (st.st_mode & S_IFDIR || (!(st.st_mode & S_IRUSR))) {
            statusCode = 403;
            return ;
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR)) {
            statusCode = 200;
            checkForFileExtension(fileName);
            return ;
        }
    }
    statusCode = 404;
}

void Response::fillBody(Request req) {
    if (statusCode == 200) {
        statusMssg += "200 OK\r\n";
        if (req.getPath() == "/") {
            body = "<!DOCTYPE html>"
                "<html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\">"
                "<input type=\"file\" name=\"file\">"
                "<button>Upload</button>"
                "</form></body></html>";
        }
        else {
            std::string buff;
            std::ifstream index(req.getPath().erase(0, 1).c_str(), std::ios::binary);
            std::ostringstream buffer;
            buffer << index.rdbuf(); //read about stream buffer
            body = buffer.str();
            index.close();
        }
    }
    else if (statusCode == 404) {
        statusMssg += "404 Not Found\r\n";
        body = "<!DOCTYPE html>"
              "<html><head>"
              "<style>"
              "h1, p {text-align:center}</style></head><body>"
              "<h1>404 Not Found</h1>"
              "<hr></hr>"
              "<p>Webserv</p>"
              "</body></html>";
    }
    else if (statusCode == 403) {
        statusMssg += "403 Forbidden\r\n";
        body = "<!DOCTYPE html>"
              "<html><head>"
              "<style>"
              "h1, p {text-align:center}</style></head><body>"
              "<h1>403 Forbidden</h1>"
              "<hr></hr>"
              "<p>Webserv</p>"
              "</body></html>";
    }
}

void Response::sendResponse(int fd, Request req) {
    char buff[150];
    std::map<std::string, std::string>::iterator it = Headers.begin();
    fillBody(req);
    finalRes += statusMssg;
    while (it != Headers.end()) {
        std::sprintf(buff, "%ld", body.size());
        setHeader("Content-Length", buff);
        std::string header = it->first + ": " + it->second;
        finalRes += header + "\r\n";
        it++;
    }
    finalRes += "\r\n";
    std::cout << body.size() << std::endl;
    if (!body.empty())
        finalRes += body;
    send(fd, finalRes.c_str(), finalRes.size(), 0);
}

