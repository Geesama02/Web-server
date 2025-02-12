/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/12 10:45:59 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::map<int, std::ifstream *> Response::files;
std::map<std::string, std::string> Response::ContentHeader;
std::map<int, Response> Response::Responses;

//constructor
Response::Response() {
    initializeContentHeader();
    Headers["Content-Type"] = "text/html";
    Headers["Connection"] = "keep-alive";
    Headers["Server"] = "Webserv";
    Headers["Content-Length"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    char curr_dirChar[120];
    getcwd(curr_dirChar, 120);
    currentDirAbsolutePath = curr_dirChar;
}

//Destructor
Response::~Response() {
    std::cout << "Response destuctor!!" << std::endl;
}

//getters
std::map<std::string, std::string>& Response::getHeadersRes() { return Headers; }
std::string Response::getQueryString() { return queryString; }
int         Response::getStatusCode() { return statusCode; };
std::string Response::getStatusMssg() { return statusMssg; };
std::string Response::getHeader( std::string key ) { return Headers[key]; };

//setters
void Response::setQueryString( std::string value ) { queryString = value; }
void Response::setStatusCode(int value) { statusCode = value; };
void Response::setStatusMssg( std::string value ) { statusMssg = value; };
void Response::setHeader( std::string key, std::string value ) { Headers[key] = value; };

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

void    Response::fromIntTochar(int number, char **buff) {
    sprintf(*buff, "%d", number);
}

void Response::notFoundResponse() {
    statusMssg += "404 Not Found\r\n";
    if (body.empty()) {
        body = "<!DOCTYPE html>"
                "<html><head><title>404 Not Found</title>"
                "<style>h1, p {text-align:center}</style></head><body>"
                "<h1>404 Not Found</h1>"
                "<hr></hr>"
                "<p>Webserv</p>"
                "</body></html>";
    }
    char buff[150];
    sprintf(buff, "%ld", body.length());
    Headers["Content-Length"] = buff;
}

void Response::forbiddenResponse() {
    statusMssg += "403 Forbidden\r\n";
    if (body.empty()) {
        body = "<!DOCTYPE html>"
            "<html><head><title>403 Forbidden</title>"
            "<style>h1, p {text-align:center}</style></head><body>"
            "<h1>403 Forbidden</h1>"
            "<hr></hr>"
            "<p>Webserv</p>"
            "</body></html>";
    }
    char buff[150];
    sprintf(buff, "%ld", body.length());
    Headers["Content-Length"] = buff;
}

void Response::successResponse(Request req) {
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
        if (!body.empty()) {
            char buff[150];
            std::sprintf(buff, "%ld", body.length());
            Headers["Content-Length"] = buff;
        }
        if (files.find(clientFd) == files.end())
            files[clientFd] = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
        Headers["Accept-Ranges"] = "bytes";
    }
}

void    Response::redirectionResponse(Request req, Config& config) {
    statusMssg += "301 Moved Permanently\r\n";
    int port = config.getClientServer()[clientFd].getPort(); 
    // char buff[120];
    // sprintf(buff, "%d", port);
    char portChar[120];
    sprintf(portChar, "%d", port);
    // fromIntTochar(port, &portChar);
    std::string host = config.getClientServer()[clientFd].getHost() + ":" + portChar;
    std::string location =  req.getPath() + "/";
    std::string locationHeader = "http://" + host + location;
    setHeader("Location", locationHeader);
    setHeader("Content-Length", "169");
}

void Response::handleRangeRequest(Request req) {
    if (files.find(clientFd) == files.end())
        files[clientFd] = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
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
        files[clientFd]->seekg(nc);
        size_t len = req.strToDecimal(Headers["Content-Length"]);
        char buff3[150];
        sprintf(buff3, "%lld", len - nc);
        setHeader("Content-Length", buff3);
    }
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

void Response::checkForQueryString(std::string& fileName) {
    size_t index = fileName.find("?");
    if (index != std::string::npos) {
        queryString = fileName.substr(index + 1);
        fileName.erase(index);
    }
}

void Response::vertifyDirectorySlash(std::string fileName) {
    size_t i = fileName.rfind("/");
    if (i != fileName.length() - 1)
        statusCode = 301;
}

void Response::searchForFile(Request req) {
    struct stat st;
    std::string fileName = req.getPath();
    char buff3[150];

    //seperating filename from querystring
    checkForQueryString(fileName);

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
            vertifyDirectorySlash(fileName);
            return ;
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR)) {
            if (req.getHeaders().find("range") != req.getHeaders().end()) {
                statusCode = 206;
                sprintf(buff3, "%ld", st.st_size);
                setHeader("Content-Length", buff3);
                checkForFileExtension(fileName);
                return ;
            }
            statusCode = 200;
            sprintf(buff3, "%ld", st.st_size);
            setHeader("Content-Length", buff3);
            if (st.st_mode & S_IFDIR)
                vertifyDirectorySlash(fileName);
            checkForFileExtension(fileName);
            return ;
        }
    }
    statusCode = 404;
}

void Response::sendBodyBytes() {
    int bytesR = 0;
    if (files.find(clientFd) != files.end()) {
        char buff[1024];
        Config::clientTimeout[clientFd] = Config::timeNow();
        files[clientFd]->read(buff, 1024);
        if (!*files[clientFd]) {
            if (files[clientFd]->eof()) {
                bytesR = files[clientFd]->gcount();
                send(clientFd, buff, bytesR, 0);
            }
            files[clientFd]->close();
            delete files[clientFd];
            files.erase(clientFd);
            return ;
        }
        bytesR = files[clientFd]->gcount();
        send(clientFd, buff, bytesR, 0);
    }
}

void Response::fillBody(Config& config, Request req) {
    if (statusCode != 301)
        checkDefinedPage(config, req);
    if (statusCode == 200)
        successResponse(req);
    else if (statusCode == 206)
        handleRangeRequest(req);
    else if (statusCode == 301)
        redirectionResponse(req, config);
    else if (statusCode == 404)
        notFoundResponse();
    else if (statusCode == 403)
        forbiddenResponse();
}

void Response::sendResponse(Config& config, Request req, int fd) {
    clientFd = fd;
    if (req.getPath().find("/cgi-bin/") != std::string::npos && statusCode == 200) {
        CGI cgiScript;
        cgiScript.execute_cgi_script(*this, clientFd, req);
        return ;
    }
    fillBody(config, req);
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

    Responses[clientFd] = *this;

    send(clientFd, finalRes.c_str(), finalRes.length(), 0);
}
