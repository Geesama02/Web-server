/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:07:04 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/08 12:47:28 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
// #include "../Response/Response.hpp"
#include "../Config/Config.hpp"
#include "../Parser/Parser.hpp"
// #include "../Config/Location.hpp"

std::map<int, std::string> Request::reqStatus;

// Constructor
Request::Request() {
    currLocation = NULL;
    headersLength = 0;
    state = 1;
    file = NULL;
}

// Getters
std::map<std::string, std::string>& Request::getHeaders() { return Headers; }
std::string Request::getMethod() { return method; }
std::string Request::getFileName() { return fileName; }
std::string& Request::getPath() { return path; }
std::string Request::getVersion() { return version; }
std::string Request::getBody() { return body; }
// std::map<int, UploadFile>& Request::getUploads() { return (uploads); }

// Setters
void Request::setMethod(std::string& m) { method = m; }
void Request::setPath(std::string p) { path = p; }
void Request::setVersion(std::string& v) { version = v; }
void Request::setBody(std::string& b) { body = b; }
void Request::addUpload(UploadFile& new_upload) {
    file = new(std::nothrow) UploadFile;
    if (!file)
      return ;  
    *file = new_upload;
    // std::string locationPath = currLocation->getUploadPath();
    // file->setPath(currLocation->getUploadPath())
}

// Destructor
Request::~Request() {
    if (file)
        delete file;
}

// Functions

std::vector<std::string> Request::split(std::string buffer, int full, char del) {
    std::stringstream s(buffer);
    std::vector<std::string> holder;
    std::string tmp_str;
    if (full) { // split full string if full == 1
        while (std::getline(s, tmp_str, del)) {
            if (!tmp_str.empty()) {
                holder.push_back(tmp_str);
            }
        }
    }
    else { // split just the first occurance of the delimeter
        if (std::getline(s, tmp_str, del)) {
            if (*tmp_str.begin() == ' ')
                tmp_str.erase(0, 1);
            to_lower(tmp_str);
            holder.push_back(tmp_str);
        }
        if (std::getline(s, tmp_str, '\r')) {
            if (*tmp_str.begin() == ' ')
                tmp_str.erase(0, 1);
            holder.push_back(tmp_str);
        }
    }
    return (holder);
}

void Request::to_lower(std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        str[i] = std::tolower(str[i]);
    }
}

int Request::checkMethod(std::string str) {
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        if (!isupper(*i))
            return (0);
    }
    return (1);
}

int Request::continueReq(std::string& buffer, size_t& stop_p) {
    if (!holder.empty()) {
        buffer = holder + buffer;
        // std::cout << "holder -> " << holder << std::endl;
        stop_p = buffer.find("\r\n\r\n");
        // if (stop_p != std::string::npos)
        //     std::cout << "FOUND\n";
        holder.clear();
    }
    // std::cout << "bef Buffer -> " << buffer << std::endl;
    if (stop_p == std::string::npos) {
        size_t last_pos = buffer.rfind("\r\n");
        if (last_pos == std::string::npos) {
            holder = buffer;
            if (holder.size() > 8192 && !state)
                return (400);
            buffer.clear();
        }
        else {
            // std::cout << "last_pos -> " << last_pos << std::endl;
            holder = buffer.substr(last_pos);
            buffer.erase(last_pos + 2);
        }
    }
    return (0);
}

std::string Request::urlDecode(std::string path) {
    size_t index = path.find('%');
    while(index != std::string::npos) {
        std::string tmp = path.substr(index + 1, 2);
        path.erase(index, 3);
        std::string c;
        c += static_cast<char>(hexToDecimal(tmp));
        path.insert(index, c.c_str());
        index = path.find('%', index + 1);
    }
    return (path);
}

int Request::checkValidPath(std::string& path) {
    std::string validChars = "-._~:/?#[]@!$&'()*+,;=%";
    for (std::string::iterator it = path.begin(); it != path.end(); it++) {
        if (!isalnum(*it) && validChars.find(*it) == std::string::npos)
            return (0);
    }
    return (1);
}

int Request::handleReqLine(std::stringstream& s) {
    std::string line;
    // std::cout << "state -> " << state << std::endl;
    if (state && std::getline(s, line)) {
        std::vector<std::string> holder;
        holder = split(line, 1, ' ');
        if (holder.size() == 2) {
            if (!checkMethod(holder[0]))
                return (400);
            if (holder[1].size() > 2048)
                return (414);
            return (400);
        }
        else if (holder.size() != 3 || (holder.size() >= 1 && !checkMethod(holder[0])))
            return (400);
        if (holder[1].size() > 2048)
            return (414);
        if (holder[2] != "HTTP/1.1\r")
            return (505);
        if (*holder[1].begin() != '/' || !checkValidPath(holder[1]))
            return (400);
        headersLength += line.size() + 1;
        setMethod(holder[0]);
        setPath(holder[1]);
        setVersion(holder[2]);
    }
    return (0);
}

int Request::parse(std::string& buffer, size_t& stop_p) {
    if (continueReq(buffer, stop_p) != 0)
        return (400);
    if (buffer.empty())
        return (2);
    std::string tmp_buff = buffer;
    if (stop_p != std::string::npos)
        tmp_buff = tmp_buff.erase(stop_p);
    std::stringstream s(tmp_buff);
    std::string line;
    // std::cout << "out buffer -> " << buffer << std::endl;
    // std::cout << "out tmp -> " << tmp_buff << std::endl;
    // if (state && buffer.find("\r\n") == std::string::npos)
    //     return (2);
    int retValue = handleReqLine(s);
    if (retValue != 0)
        return (retValue);
    // std::cout << "after retValue" << std::endl;
    while (std::getline(s, line)) {
        if (line == "\r")
            continue;
        // std::cout << "INSIIIDE---------------------------\n";
        headersLength += line.size();
        std::vector<std::string> holder = split(line, 0, ':');
        // std::cout << "line -> " << line << std::endl;
        // std::cout << "size -> " << holder.size() << std::endl;
        if (holder.size() != 2 || line.size() >= 8192 || headersLength > 32768)
            return (400);
        Headers[holder[0]] = holder[1];
    }
    // std::cout << "out line -> " << line<<std::endl;
    if (stop_p == std::string::npos) {
        state = 0;
        return (2);
    }
    else
        state = 1;
    if (method != "GET" && method != "POST" && method != "DELETE")
        return (405);
    return (0);
}

int Request::handleFirstPart(UploadFile& file, std::string& str) {
    size_t namePos = str.find("filename=\"");
    if (namePos != std::string::npos) {
        size_t nameStart = namePos + 10;
        size_t nameEnd = str.find("\"", nameStart);
        if (nameEnd != std::string::npos) {
            file.setFilename(str.substr(nameStart, nameEnd - nameStart));
            if (file.getFilename().empty())
                return (1);
            if (!file.openFile())
                return (500);
            size_t stop_p = str.find("\r\n\r\n");
            if (stop_p != std::string::npos) {
                file.setExpectedBytes(file.getExpectedBytes() - (stop_p + 4));
                str = str.substr(stop_p + 4);
                return (handleFilePart(file, str));
            }
        }
    }
    return (0);
}

int Request::handleFilePart(UploadFile& file, std::string& str) {
    size_t stopPos;
    if ((stopPos = str.find("\r\n--" + file.getBoundary() + "--")) != std::string::npos) {
        str.erase(stopPos);
        *file.getFd() << str;
        file.setState(true);
        fileName = file.getFilename();
        return (201);
    }
    if (!str.empty()) {
        std::string toWrite = str.substr(0, file.getExpectedBytes() - (file.getBoundary().size() - 6)); // write exactly the file binary length
        *file.getFd() << toWrite;
        file.setExpectedBytes(file.getExpectedBytes() - toWrite.size());
        if (toWrite.size() < str.size()) {
            str = str.substr(toWrite.size());
            file.setTmpContent(str);
        }
    }
    return (0);
}

int Request::readFile(UploadFile& file, std::string str) {
    if (file.getTmpContent() != "") { // merge saved input with new file binary
        str = file.getTmpContent() + str;
        file.setTmpContent("");
    }
    if (str.compare(0, file.getBoundary().size() + 2, "--" + file.getBoundary()) == 0)
        return (handleFirstPart(file, str));
    return (handleFilePart(file, str));
}

long long Request::hexToDecimal(std::string str) {
    std::stringstream s(str);
    long long holder;
    
    s >> std::hex >> holder;
    return (holder);
}

long long Request::strToDecimal(std::string str) {
    std::stringstream s(str);
    long long holder;
    
    s >> holder;
    return (holder);
}

int Request::writeFirstChunk(UploadFile& file, std::string& str) {
    size_t endPos = str.find("\r\n");
    if (endPos != std::string::npos) {
        std::string toRead = str.substr(0, endPos);
        file.setExpectedBytes(hexToDecimal(toRead));
        if (file.getExpectedBytes() <= 0) {
            if (file.getExpectedBytes() == 0) {
                file.setState(true);
                fileName = file.getFilename();
                return (201);
            }
            return (1);
        }
        str = str.substr(endPos + 2);
        if (!file.getFd() && !file.openFile())
            return (500);
        return (writeFile(file, str));
    }
    return (0);
}

int Request::checkChunks(UploadFile& file, std::string& str) {
    size_t startPos = str.find("\r\n");
    if (startPos != std::string::npos && file.getExpectedBytes() == 0) {
        startPos = startPos + 2;
        size_t endPos = str.find("\r\n", startPos);
        if (endPos != std::string::npos) {
            std::string toRead = str.substr(startPos, endPos - startPos);
            file.setExpectedBytes(hexToDecimal(toRead));
            if (file.getExpectedBytes() <= 0) {
                if (file.getExpectedBytes() == 0) {
                    file.setState(true);
                    fileName = file.getFilename();
                    return (201);
                }
                return (1);
            }
            str = str.substr(endPos + 2);
        }
        else 
            file.setTmpContent(str.substr(startPos)); // if chunk size is cut off
    }
    return (0);
}

int Request::writeFile(UploadFile& file, std::string& str) {
    std::string toWrite = str.substr(0, file.getExpectedBytes());
    *file.getFd() << toWrite;
    file.setExpectedBytes(file.getExpectedBytes() - toWrite.size());
    if (toWrite.size() < str.size()) {
        str = str.substr(toWrite.size());
        if (str == "\r\n0\r\n\r\n") { // all chunks received
            file.setState(true);
            fileName = file.getFilename();
            return (201);
        }
        if (file.getExpectedBytes() == 0) // save the new chunk to handle with next request
            file.setTmpContent(str);
    }
    return (0);
}

int Request::readBinaryFile(UploadFile& file, std::string str) {
    if (file.isFirstRead()) {
        file.setFirstRead(false);
        file.setFilename(file.getFilename() + getExtension(Headers["content-type"]));
        if (!file.openFile())
            return (500);
    }
    std::string toWrite = str.substr(0, file.getExpectedBytes());
    *file.getFd() << toWrite;
    file.setExpectedBytes(file.getExpectedBytes() - toWrite.size());
    if (file.getExpectedBytes() <= 0) {
        file.setState(true);
        fileName = file.getFilename();
        return (201);
    }
    return (0);
}
int Request::readChunkedFile(UploadFile& file, std::string str) {
    if (file.getTmpContent() != "") { // merge saved input with new file binary
        str = file.getTmpContent() + str;
        file.setTmpContent("");
    }
    if (file.isFirstRead() && !str.empty() && file.getExpectedBytes() == 0) { // get chunk size and write it
        file.setFirstRead(false);
        return (writeFirstChunk(file, str));
    }
    else if (!str.empty()) {
        int status = 0;
        if ((status = checkChunks(file, str)) != 0 || (status = writeFile(file, str)) != 0) // write chunk and check if there's other chunks
            return (status);
    }
    return (0);
}

int Request::setupPostBody() {
    UploadFile file;
    if (Headers.find("content-length") != Headers.end())
        file.setExpectedBytes(strToDecimal(Headers["content-length"]));
    else
        return (411); // Missing Content-length
    file.setType("post");
    file.setFilename(".tmp");
    char *user = std::getenv("USER");
    if (user) {
        std::string userStr = user;
        file.setPath("/home/" + userStr + "/goinfre/");
    }
    else
        file.setPath("/tmp/");
    addUpload(file);
    if (!this->file)
        return (500);
    return (0);
}

int Request::continuePostBody(std::string str) {
    int status = 0;
    status = readBinaryFile(*file, str);
    if (status != 0) {
        if (status == 201) {
            status = 0;
            fileName = file->getFilename();
        }
        delete file;
        file = NULL;
    }
    else
        return (2);
    return (status);
}

int Request::handleFiles(std::string& str) {
    int status = 0;
    if (!file)
        return (status);
    if (file->getType() == "multipart") {
        status = readFile(*file, str);
        if (status == 0)
            return (2);
    }
    else if (file->getType() == "chunked") {
        status = readChunkedFile(*file, str);
        if (status == 0)
            return (2);
    }
    else if (file->getType() == "binary") {
        status = readBinaryFile(*file, str);
        if (status == 0)
            return (2);
    }
    if (status != 0) {
        delete file;
        file = NULL;
    }
    return (status);
}

std::string Request::getExtension(std::string type) {
    for (std::map<std::string, std::string>::iterator it = Response::ContentTypeHeader.begin(); it != Response::ContentTypeHeader.end(); it++) {
        if (type == it->second)
            return (it->first);
    }
    return ("");
}

int Request::setupBinaryFile() {
    UploadFile file;
    if (Headers.find("content-length") != Headers.end())
        file.setExpectedBytes(strToDecimal(Headers["content-length"]));
    else
        return (411); // Missing Content-length
    file.setType("binary");
    int ret = joinPath(currLocation, file);
    if (ret != 0)
        return (ret);
    addUpload(file);
    if (!this->file)
        return (500);
    return (0);
}

int Request::joinPath(Location* location, UploadFile& file) {
    if (!location || location->getUploadPath().empty())
        return (403);
    std::string locationPath = location->getUploadPath();
    std::string locationRoot = location->getRoot();
    if (*locationRoot.rbegin() != '/')
        locationRoot += '/';
    if (*locationPath.rbegin() != '/')
        locationPath += '/';
    if (*locationPath.begin() == '/')
        locationPath.erase(0, 1);
    // std::cout << "Path -> " << locationRoot + locationPath << std::endl;
    file.setPath(locationRoot + locationPath);
    return (0);
}

int Request::handlePostReq() {
    if (method == "POST" 
        && Headers.find("content-type") != Headers.end()
        && Headers["content-type"].find("boundary=") != std::string::npos) {
        return (setupFile());
    }
    else if (method == "POST" 
            && Headers.find("transfer-encoding") != Headers.end()) {
        if (Headers["transfer-encoding"] == "chunked")
            return (setupChunkedFile());
        else
            return (501);
    }
    else if (method == "POST" 
        && Headers.find("content-type") != Headers.end()
        && Headers["content-type"] != "text/plain"
        && Headers["content-type"] != "application/x-www-form-urlencoded") {
        return (setupBinaryFile());
    }
    else if (method == "POST"
            && Headers.find("content-length") != Headers.end()) {
        return (setupPostBody());
    }
    else if (method == "POST")
        return (400);
    return (0);
}

int Request::isNumber(std::string& str)
{
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        if (!std::isdigit(*i))
            return (0);
    }
    return (1);
}

Server Request::getServer(Server& server, std::vector<Server>& Servers) {
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++)
    {
        if (it->getPort() == server.getPort() && it->getHost() == server.getHost()) {
            std::string tmp = Headers["host"];
            size_t pos = tmp.rfind(':');
            if (pos != std::string::npos)
                tmp.erase(pos);
            for(std::vector<std::string>::iterator it2 = it->getServerName().begin(); it2 != it->getServerName().end(); it2++) {
                if (tmp == *it2) {
                    Server tmp_server = *it;
                    tmp_server.setSocket(-1);
                    return (tmp_server);
                }
            }
        }
    }
    return (server);
}

void Request::clearReq() {
    Headers.clear();
    fileName.clear();
    method.clear();
    path.clear();
    version.clear();
    headersLength = 0;
    currLocation = NULL;
}

Location *Request::getMatchedLocation(std::string path, Server& server) {
    Location *loc = NULL;
    for (std::vector<Location>::iterator it = server.getLocations().begin(); it != server.getLocations().end(); it++) {
        Response::matchReqPathWithLocation(*it, path, &loc);
    }
    return (loc);
}

int Request::checkAllowedMethods(std::string& str, Server& server) {
    if (currLocation) {
        for (std::vector<std::string>::iterator it = currLocation->getAllowedMethods().begin(); it != currLocation->getAllowedMethods().end(); it++) {
            if (*it == method) {
                if (!str.empty() && method == "DELETE")
                    return (415);
                return (0);
            }
        }
        if (method == "DELETE" && !currLocation->getMethodsFlag())
            return (405);
        return (403);
    }
    else {
        if (method == "DELETE")
            return (405);
        if (!server.getCgiDir().empty() && method == "POST" && strncmp(server.getCgiDir().c_str(), path.c_str(), server.getCgiDir().size()) == 0)
            return (0);
        if (method != "GET")
            return (403);
    }
    return (0);
}

int Request::readHeaders(std::string& str, Server& server, std::vector<Server>& Servers) {
    if (!file) {
        int status;
        if (state)
            clearReq();
        size_t stop_p = str.find("\r\n\r\n");
        if ((status = parse(str, stop_p)) != 0)
            return (status);
        // std::cout <<  "OUT\n";
        if (Headers.find("host") != Headers.end())
            server = getServer(server, Servers);
        // std::cout << "stop_p -> " << stop_p << std::endl;
        str = str.substr(stop_p + 4);
        currLocation = getMatchedLocation(path, server);
        if (currLocation)
            std::cout << "FOUND LOCATION -> " << currLocation->getURI() << std::endl;
        if ((status = checkAllowedMethods(str, server)) != 0)
            return (status);
        if (Headers.find("host") == Headers.end() || (method == "POST"
            && Headers.find("content-length") != Headers.end()
            && !isNumber(Headers["content-length"])))
            return (std::cout << "inside bad req\n",400); // Bad Request
        else if (method == "POST"
            && Headers.find("content-length") != Headers.end() && currLocation
            && strToDecimal(Headers["content-length"]) > currLocation->getClientMaxBodySize()
            && currLocation->getClientMaxBodySize() != 0)
            return (413); // Request Too Big
        int res = handlePostReq();
        if (res != 0)
            return (res);
    }
    if (file && file->getType() == "post")
        return (continuePostBody(str));
    else
        return (handleFiles(str));
}

int Request::setupChunkedFile() {
    UploadFile file;
    
    file.setType("chunked");
    addUpload(file);
    if (!this->file)
        return (500);
    int ret = joinPath(currLocation, file);
    if (ret != 0)
        return (ret);
    return (0);
}

int Request::setupFile() {
    UploadFile file;
    if (Headers.find("content-length") != Headers.end())
        file.setExpectedBytes(strToDecimal(Headers["content-length"]));
    else
        return (411); // Missing Content-length
    size_t pos = Headers["content-type"].find("boundary=");
    std::string boundary = Headers["content-type"].substr(pos + 9);
    file.setType("multipart");
    file.setBoundary(boundary);
    int ret = joinPath(currLocation, file);
    if (ret != 0)
        return (ret);
    addUpload(file);
    if (!this->file)
        return (500);
    return (0);
}

int Request::readRequest(int fd, Server& server, std::vector<Server>& Servers) {
    char buff[4096];
    std::string str;
    ssize_t received = recv(fd, buff, sizeof(buff) - 1, 0);
    if (received <= 0)
        return (1);
    else {
        buff[received] = '\0';
        str.append(buff, received);
        // std::cout << "received: " << str << std::endl;
        return (readHeaders(str, server, Servers));
    }
    return (0);
}
