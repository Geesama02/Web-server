/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/28 10:36:06 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../Config/Config.hpp"
#include "../Parser/Parser.hpp"

//redirect response just from 304 and above

// std::map<int, std::ifstream *> Response::files;
std::map<std::string, std::string> Response::ContentTypeHeader;

//constructor
Response::Response()
{
    FileType = 0;
    redirectFlag = 0;
    initializeContentHeader();
    initializeStatusRes();
    file = NULL;
    indexFile = NULL;
    errorPage = NULL;
    Headers["Content-Type"] = "text/html";
    Headers["Connection"] = "keep-alive";
    Headers["Server"] = "Webserv";
    Headers["Content-Length"] = "0";
    Headers["Date"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    char curr_dirChar[120];
    getcwd(curr_dirChar, 120);
    currentDirAbsolutePath = curr_dirChar;
}

//Destructor
Response::~Response() 
{
   clearResponse(); 
}

//getters
std::map<std::string, std::string>& Response::getHeadersRes() { return Headers; }
int                                 Response::getClientFd() { return clientFd; }
std::string                         Response::getQueryString() { return queryString; }
int                                 Response::getStatusCode() { return statusCode; }
std::string                         Response::getStatusMssg() { return statusMssg; }
std::string                         Response::getHeader( std::string key ) { return Headers[key]; };
std::ifstream&                      Response::getFile() { return *file; }
std::ifstream&                      Response::getIndexFile() { return *indexFile; }

//setters
void                                Response::setClientFd( int nFd ) { clientFd = nFd; }
void                                Response::setQueryString( std::string value ) { queryString = value; }
void                                Response::setStatusCode(int value) { statusCode = value; };
void                                Response::setStatusMssg( std::string value ) { statusMssg = value; };
void                                Response::setHeader( std::string key, std::string value ) { Headers[key] = value; };
void                                Response::setFile(std::ifstream *nFile) { file = nFile; }
void                                Response::setIndexFile(std::ifstream *nIndexFile) { indexFile = nIndexFile; }


//other
void Response::initializeStatusRes()
{
    locationMatch = NULL;
    errorPage = NULL;
    indexFile = NULL;
    file = NULL;
    resStatus.insert(std::make_pair(200, "OK\r\n"));
    resStatus.insert(std::make_pair(201, "Created\r\n"));
    resStatus.insert(std::make_pair(204, "No Content\r\n"));
    resStatus.insert(std::make_pair(206, "Partial Content\r\n"));
    resStatus.insert(std::make_pair(301, "Moved Permanently\r\n"));
    resStatus.insert(std::make_pair(302, "Moved Temporarily\r\n"));
    resStatus.insert(std::make_pair(400, "Bad Request\r\n"));
    resStatus.insert(std::make_pair(403, "Forbidden\r\n"));
    resStatus.insert(std::make_pair(404, "Not Found\r\n"));
    resStatus.insert(std::make_pair(405, "Method Not Allowed\r\n"));
    resStatus.insert(std::make_pair(409, "Conflict\r\n"));
    resStatus.insert(std::make_pair(411, "Length Required\r\n"));
    resStatus.insert(std::make_pair(413, "Content Too Large\r\n"));
    resStatus.insert(std::make_pair(414, "URI Too Long\r\n"));
    resStatus.insert(std::make_pair(415, "Unsupported Media Type\r\n"));
    resStatus.insert(std::make_pair(444, "No Response\r\n"));
    resStatus.insert(std::make_pair(500, "Internal Server Error\r\n"));
    resStatus.insert(std::make_pair(501, "Not Implemented\r\n"));
    resStatus.insert(std::make_pair(502, "Bad Gateway\r\n"));
    resStatus.insert(std::make_pair(504, "Gateway Timeout\r\n"));
}

void Response::initializeContentHeader()
{
    ContentTypeHeader[".txt"] = "text/plain";
    ContentTypeHeader[".html"] = "text/html";
    ContentTypeHeader[".css"] = "text/css";
    ContentTypeHeader[".js"] = "application/javascript";
    ContentTypeHeader[".pdf"] = "application/pdf";
    ContentTypeHeader[".png"] = "image/png";
    ContentTypeHeader[".jpg"] = "image/jpeg";
    ContentTypeHeader[".jpeg"] = "image/jpeg";
    ContentTypeHeader[".mp4"] = "video/mp4";
    ContentTypeHeader[".mp3"] = "audio/mpeg";
    ContentTypeHeader[".json"] = "application/json";
}


void    Response::addHeadersToResponse()
{
    std::map<std::string, std::string>::iterator it = Headers.begin();
    while (it != Headers.end()) {
        std::string header = it->first + ": " + it->second;
        finalRes += header + "\r\n";
        it++;
    }
}

void    Response::clearResponse()
{
    Headers.clear();
    statusMssg.clear();
    Headers["Connection"] = "keep-alive";
    Headers["Content-Length"] = "0";
    Headers["Server"] = "Webserv";
    Headers["Date"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    FileType = 0;
    redirectFlag = 0;
    locationMatch = NULL;
    body.clear();
    finalRes.clear();
    if (file)
    {
        file->close();
        delete file;
    }
    if (indexFile)
    {
        indexFile->close();
        delete indexFile;
    }
    if (errorPage)
    {
        errorPage->close();
        delete errorPage;
    }
    file = NULL;
    errorPage = NULL;
    indexFile = NULL;
}

std::string Response::getDate()
{
    time_t timestamp = time(NULL);
    struct tm datetime = *gmtime(&timestamp);
    char now[50];

    strftime(now, 50, "%a, %d %b %Y %H:%M:%S GMT", &datetime);
    std::string res = now;
    return (res);
}

void Response::generateRes(Config& config)
{
    char buff[150];
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    if (body.empty() && statusCode != 204)
    {
        body = "<!DOCTYPE html>"
                "<html>"
                "<head><title>" + statusCodeStr + " " + resStatus[statusCode] + "</title></head>"
                "<body>"
                "<center><h1>" + statusCodeStr + " " + resStatus[statusCode] + "</h1></center>"
                "<hr><center>Webserv</center>"
                "</body>"
                "</html>";
    }
    if (!redirectFlag && statusCode != 204)
    {
        char contentLength[150];
        std::sprintf(contentLength, "%ld", body.length());
        Headers["Content-Length"] = contentLength;
        Headers["Content-Type"] = "text/html";
    }
    Headers["Date"] = getDate();
    if (!redirectFlag && statusCode == 201)
        Headers["Location"] = config.getClients()[clientFd].getRequest().getFileName();
    if (!redirectFlag && statusCode >= 500)
        Headers["Connection"] = "close";
}

void Response::successResponse(Request req)
{
    char contentLengthHeader[150];
    statusMssg += "200 OK\r\n";
    
    if (!body.empty())
    {
        std::sprintf(contentLengthHeader, "%ld", body.length());
        Headers["Content-Length"] = contentLengthHeader;
    }
    if (!file && FileType)
    {
        Headers["Accept-Ranges"] = "bytes";
        file = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
    }
    Headers["Date"] = getDate();
}

void    Response::redirectionResponse(Request req, Config& config)
{
    char buff[150];
    char portChar[150];
    int  port;
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    port = config.getClients()[clientFd].getServer().getPort(); 
    sprintf(portChar, "%d", port);
    std::string host = config.getClients()[clientFd].getServer().getHost() + ":" + portChar;
    if (body.empty())
    {
        body = "<!DOCTYPE html>"
                "<html>"
                "<head><title>" + statusCodeStr + " " + resStatus[statusCode] + "</title></head>"
                "<body>"
                "<center><h1>" + statusCodeStr + " " + resStatus[statusCode] + "</h1></center>"
                "<hr><center>Webserv</center>"
                "</body>"
                "</html>";
    }
    if (statusCode == 301 && locationHeader.length() == 0)
        locationHeader = "http://" + host + req.getPath() + "/";
    else if (statusCode == 301 && locationHeader.length() > 0)
        locationHeader = "http://" + host + locationHeader + "/";
    std::cout << "location header  -> " << locationHeader << std::endl;
    setHeader("Location", locationHeader);
    char contentLengthHeader[150];
    std::sprintf(contentLengthHeader, "%ld", body.length());
    Headers["Content-Length"] = contentLengthHeader;
    Headers["Date"] = getDate();
}

void Response::rangeResponse(Request req) {
    if (!file)
        file = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
    char buff[150];
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    std::string range = req.getHeaders()["range"];
    size_t i = range.find("=");
    if (i == std::string::npos)
        return ;
    range.replace(i, 1, " ");
    if (getHeader("Content-Type") == "video/mp4"
            || getHeader("Content-Type") == "audio/mpeg") {
        char buff2[150];
        size_t length = req.strToDecimal(Headers["Content-Length"]);
        sprintf(buff2, "%ld", length - 1);
        Headers["Content-Range"] = range + buff2 + '/' + Headers["Content-Length"]; // construct the header content-range with the corresponding values
        std::string rangeNumber = range.substr(i, range.size() - i);
        long long rangeStart = req.strToDecimal(rangeNumber);
        file->seekg(rangeStart);  //this is where we start sending the video content
        size_t len = req.strToDecimal(Headers["Content-Length"]);
        char rangeContentLength[150];
        sprintf(rangeContentLength, "%lld", len - rangeStart);
        setHeader("Content-Length", rangeContentLength);
        Headers["Date"] = getDate();
    }
}

void Response::checkForFileExtension(std::string extension)
{
    size_t pos = extension.rfind(".");
    if (pos != std::string::npos) {
        extension.erase(0, pos);
        std::map<std::string, std::string>::iterator it = ContentTypeHeader.begin();
        while(it != ContentTypeHeader.end())
        {
            if (it->first == extension) {
                setHeader("Content-Type", it->second);
                return ;
            }
            it++;
        }
    }
    setHeader("Content-Type", "application/stream-octet");
}

void Response::checkForQueryString(std::string& fileName)
{
    size_t index = fileName.find("?");
    if (index != std::string::npos)
    {
        queryString = fileName.substr(index + 1);
        fileName.erase(index);
    }
}

void Response::verifyDirectorySlash(std::string fileName, Request& req)
{
    if (fileName.rfind("/") != fileName.length() - 1)
    {
        if (req.getMethod() == "DELETE")
            statusCode = 409;
        else
            statusCode = 301;
    }
    else if (req.getMethod() == "DELETE")
        statusCode = 204;
}

void Response::searchForFile(Config& config, Request& req)
{
    struct stat st;
    std::string fileName;
    std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
    char buff3[150];

    //seperating filename from querystring
    checkForQueryString(fileName);
    if (fileName == "/")
        fileName = serverRoot;
    else if (serverRoot.length() > 0 && serverRoot.rfind("/") == serverRoot.length() - 1) 
        serverRoot.erase(serverRoot.length() - 1);
    fileName = serverRoot + req.getPath();
    req.setPath(req.urlDecode(req.getPath()));
    fileName = req.urlDecode(fileName); 

    std::cout << "file request -> " << fileName << std::endl;
  
    if (!stat(fileName.c_str(), &st))
    {
        if (st.st_mode & S_IFDIR || (!(st.st_mode & S_IRUSR)))
        {
            statusCode = 403;
            returnResponse(config);
            if (redirectFlag)
                return ;
            verifyDirectorySlash(fileName, req);
            if (statusCode == 403)
                checkAutoIndex(config, req);
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR))
        {
            if (req.getHeaders().find("range") != req.getHeaders().end())
            {
                statusCode = 206;
                sprintf(buff3, "%ld", st.st_size);
                setHeader("Content-Length", buff3);
                checkForFileExtension(fileName);
            }
            else
            {
                returnResponse(config);
                if (redirectFlag)
                    return ;
                if (req.getMethod() == "DELETE")
                    statusCode = 204;
                else
                    statusCode = 200;
                if (statusCode == 200)
                    FileType = 1;
                sprintf(buff3, "%ld", st.st_size);
                setHeader("Content-Length", buff3);
                checkForFileExtension(fileName);
            }
        }
    }
    else
        statusCode = 404;
}

int Response::sendBodyBytes()
{
    int bytesR = 0;
    if (file) {
        char buff[1024];
        // if marouan updates, update timeout of client here ---------
        file->read(buff, 1024);
        if (file->eof()) 
        {
            bytesR = file->gcount();
            if (send(clientFd, buff, bytesR, 0) == -1)
            {
                std::cerr << "Error : Send Fail" << std::endl;
                return (-1);
            }
            file->close();
            delete file;
            file = NULL;
            return (0);
        }
        bytesR = file->gcount();
        if (send(clientFd, buff, bytesR, 0) == -1)
        {
            std::cerr << "Error : Send Fail" << std::endl;
            return (-1);
        }
    }
    return (0);
}

void Response::handleDeleteRequest(Config& config, Request& req)
{
  std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
  std::string requestedPath;

  if (serverRoot.length() > 0 && serverRoot.rfind("/") != serverRoot.length() - 1)
      requestedPath = serverRoot + "/" + req.getPath();
  else
      requestedPath = serverRoot + req.getPath();
  char requestPath[200];
  std::strcpy(requestPath, requestedPath.c_str());
  if (statusCode == 204)
  {
      if (rmrf(requestPath) == -1)
      {
          clearResponse();
          statusCode = 500;
          return ;
      }      
  }
  Headers.erase("Content-Type");
  Headers.erase("Content-Length");
}

void Response::fillBody(Config& config, Request& req)
{
    if (!redirectFlag)
        checkErrorPages(config, req);
    if (statusCode == 200)
        successResponse(req);
    else if (statusCode == 206)
        rangeResponse(req);
    else if (statusCode == 301 || statusCode == 302)
        redirectionResponse(req, config);
    else
        generateRes(config);
}

void Response::sendResponse(Config& config, Request& req, int fd)
{
    if (statusCode == 204)
        handleDeleteRequest(config, req);
    else if (req.getPath().find("/cgi-bin/") != std::string::npos && statusCode == 200)
    {
        int status = 0;
        status = config.getClients()[fd].getCGI().execute_cgi_script(config, *this, clientFd, req);
        if (fd != 0)
        {
            config.checkCgiScriptExecution(fd);
            config.checkScriptTimeOut(fd);
        }
        if (!config.getClients()[fd].getCGI().getChildStatus() && !status)
            return ;
    }
    fillBody(config, req);
    finalRes += statusMssg;

    //add headers to final response
    addHeadersToResponse();

    // add body to final response
    finalRes += "\r\n";
    if (!body.empty())
        finalRes += body;
    send(clientFd, finalRes.c_str(), finalRes.length(), 0);
    if (statusCode >= 500)
        config.closeConnection(fd);
}
