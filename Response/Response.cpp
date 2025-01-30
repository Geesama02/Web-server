/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/30 18:24:38 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <string>
#include <sstream>

std::map<int, std::ifstream *> Response::files;

//constructor
Response::Response() {
    initializeContentHeader();
    initializeExecutablePaths();
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
    ContentHeader[".mp3"] = "audio/mpeg";
    ContentHeader[".json"] = "application/json";
}

void Response::initializeExecutablePaths() {
    executablePaths[".py"] = "/usr/bin/python3";
    executablePaths[".php"] = "/usr/bin/php";
}

void Response::notFoundResponse() {
    statusMssg += "404 Not Found\r\n";
    body = "<!DOCTYPE html>"
            "<html><head>"
            "<style>h1, p {text-align:center}</style></head><body>"
            "<h1>404 Not Found</h1>"
            "<hr></hr>"
            "<p>Webserv</p>"
            "</body></html>";
    char buff[150];
    sprintf(buff, "%ld", body.length());
    Headers["Content-Length"] = buff;
}

void Response::forbiddenResponse() {
    statusMssg += "403 Forbidden\r\n";
    body = "<!DOCTYPE html>"
            "<html><head>"
            "<style>h1, p {text-align:center}</style></head><body>"
            "<h1>403 Forbidden</h1>"
            "<hr></hr>"
            "<p>Webserv</p>"
            "</body></html>";
    char buff[150];
    sprintf(buff, "%ld", body.length());
    Headers["Content-Length"] = buff;
}

void Response::successResponse(Request req, int fd) {
    statusMssg += "200 OK\r\n";
    if (req.getPath() == "/") {
        body = "<!DOCTYPE html>"
            "<html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\">"
            "<input type=\"file\" name=\"file\">"
            "<button>Upload</button>"
            "</form></body></html>";
        char buff[150];
        std::sprintf(buff, "%ld", body.length());
        Headers["Content-Length"] = buff;
    }
    else {
        if (files.find(fd) == files.end())
            files[fd] = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
        Headers["Accept-Ranges"] = "bytes";
    }
}

void Response::handleRangeRequest(Request req, int fd) {
    if (files.find(fd) == files.end())
        files[fd] = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
    statusMssg = "HTTP/1.1 206 Partial Content\r\n";
    std::string range = req.getHeaders()["range"];
    size_t i = range.find("=");
    if (i == std::string::npos)
        return ;
    range.replace(i, 1, " ");
    if (getHeader("Content-Type") == "video/mp4"
        || getHeader("Content-Type") == "audio/mpeg") {
        char buff2[150];
        size_t length = req.strToDecimal(Headers["Content-Length"]);
        std::sprintf(buff2, "%ld", length - 1);
        Headers["Content-Range"] = range + buff2 + '/' + Headers["Content-Length"];
        std::string test = range.substr(i, range.size() - i);
        long long nc = req.strToDecimal(test);
        files[fd]->seekg(nc);
        size_t len = req.strToDecimal(Headers["Content-Length"]);
        char buff3[150];
        sprintf(buff3, "%lld", len - nc);
        setHeader("Content-Length", buff3);
    }
}

void Response::checkForFileExtension(Request req, std::string extension) {
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
    if (req.getPath().find("/cgi-bin/") != std::string::npos)
        setHeader("Content-Type", "text/html");
    else
        setHeader("Content-Type", "application/stream-octet");
}

void Response::searchForFile(Request req) {
    struct stat st;
    std::string fileName = req.getPath();
    char buff3[150];

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
            if (req.getHeaders().find("range") != req.getHeaders().end()) {
                statusCode = 206;
                sprintf(buff3, "%ld", st.st_size);
                setHeader("Content-Length", buff3);
                checkForFileExtension(req, fileName);
                return ;
            }
            statusCode = 200;
            sprintf(buff3, "%ld", st.st_size);
            setHeader("Content-Length", buff3);
            checkForFileExtension(req, fileName);
            return ;
        }
    }
    statusCode = 404;
}

void Response::sendBodyBytes(int fd) {
    if (files.find(fd) != files.end()) {
        char buff[1024];
        files[fd]->read(buff, 1024);
        if (!*files[fd]) {
            if (files[fd]->eof()) {
                std::cout << "transmitting bytes finished" << std::endl;
                int bytesR = files[fd]->gcount();
                send(fd, buff, bytesR, 0);
            }
            files[fd]->close();
            delete files[fd];
            files.erase(fd);
            return ;
        }
        int bytesR = files[fd]->gcount();
        send(fd, buff, bytesR, 0);
    }
}

void Response::fillBody(Request req, int fd) {
    if (statusCode == 200)
        successResponse(req, fd);
    else if (statusCode == 206)
        handleRangeRequest(req, fd);
    else if (statusCode == 404)
        notFoundResponse();
    else if (statusCode == 403)
        forbiddenResponse();
}

void Response::execute_cgi_script(int fd, Request req, char **envp) {
    int save_out = dup(1);
    int fds[2];
    if (pipe(fds) != 0)
        std::cerr << "pipe failed!!" << std::endl;
    int c_fd = fork();
    if (c_fd == 0) {
        //set environment variables 
        char contentLengthStr[150];
        if (req.getBody().length() > 0)
            sprintf(contentLengthStr, "%ld", req.getBody().length());
        setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
        setenv("SCRIPT_NAME", req.getPath().empty() ? "/" : req.getPath().c_str(), 1);
        setenv("SERVER_NAME", "Webserv", 1);
        setenv("SERVER_PROTOCOL", "HTTP 1.1" , 1);
        setenv("CONTENT_LENGTH", req.getBody().length() > 0 ? contentLengthStr : "", 1);
        setenv("CONTENT_TYPE", req.getHeaders()["content-type"].c_str(), 1);
    
        // extract the script path and extension
        std::string scriptPath = getenv("SCRIPT_NAME");
        int index = scriptPath.rfind(".");
        int index2 = scriptPath.rfind("/");
        std::string extensionFile = scriptPath.erase(0, index);
        std::string fileName = scriptPath.erase(0, index2);
        fileName.erase(0, 1);

        //find the executable path
        std::map<std::string, std::string>::iterator it = executablePaths.begin();
        while (it->first != extensionFile)
            it++;

        //find the absolute path of the script
        char buff[120];
        getcwd(buff, 120);
        std::string current_dir = buff;
        std::string relativePath = getenv("SCRIPT_NAME");
        std::string abosluteFilePath = current_dir + relativePath;
        char *argt[3];
        char executablePath[it->second.length() + 1];
        char scriptName[abosluteFilePath.length() + 1];
        std::strcpy(executablePath, it->second.c_str());
        std::strcpy(scriptName, abosluteFilePath.c_str());
        argt[0] = executablePath;
        argt[1] = scriptName;
        argt[2] = NULL;

        //status response status
        // if (statusCode == 200)
        //     statusMssg += "200 OK\r\n";
        // finalRes += statusMssg;
        //set response headers
        // setHeader("Content-Type", getenv("CONTENT_TYPE"));
        // setHeader("Content-Type", "text/html");
        // setHeader("Server", getenv("SERVER_NAME"));
        //set content length
        // struct stat st;
        // if (!stat(scriptName, &st))
        //     std::cerr << "Stat failed!!" << std::endl;
        // std::map<std::string, std::string>::iterator it = Headers.begin();
        // std::string header;
        // while (it != Headers.end()) {
        //     if (it->first != "Content-Length") {
        //         header = it->first + ": " + it->second;
        //         finalRes += header + "\n";
        //     }
        //     it++;
        // }
        // finalRes += "\n";
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);
        std::cout << finalRes << std::endl;
        execve(executablePath, argt, envp);
        dup2(save_out, 1);
        close(save_out);
    }
    close(fds[1]);
    close(c_fd);
    close(save_out);
    waitpid(-1, NULL, 0);

    //read the ouput of the script executed by the cgi
    char buff2[1025];
    std::string cgiRes;
    cgiRes += statusMssg;
    size_t nbytes = read(fds[0], buff2, 1024);
    buff2[nbytes] = '\0';
    cgiRes += buff2;
    while ((nbytes = read(fds[0], buff2, 1024)) > 0) {
        buff2[nbytes] = '\0';
        cgiRes += buff2;
    }
    close(fds[0]);
    send(fd, cgiRes.c_str(), cgiRes.length(), 0);
}

void Response::sendResponse(int fd, Request req, char **envp) {
    if (req.getPath().find("/cgi-bin/") != std::string::npos) {
        execute_cgi_script(fd, req, envp);
        return ;
    }
    fillBody(req, fd);
    finalRes += statusMssg;
    std::map<std::string, std::string>::iterator it = Headers.begin();
    while (it != Headers.end()) {
        std::string header = it->first + ": " + it->second;
        finalRes += header + "\r\n";
        it++;
    }
    finalRes += "\r\n";
    if (!body.empty())
        finalRes += body;
    send(fd, finalRes.c_str(), finalRes.length(), 0);
}
