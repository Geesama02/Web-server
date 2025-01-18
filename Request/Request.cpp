/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:07:04 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/18 13:52:44 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

std::map<int, UploadFile> Request::uploads;
std::map<int, Request> Request::unfinishedReqs;

// Constructor
Request::Request() : contentLength(0) {}

// Getters
std::map<std::string, std::string>& Request::getHeaders() { return Headers; }
std::string Request::getMethod() { return method; }
std::string Request::getPath() { return path; }
std::string Request::getVersion() { return version; }
std::string Request::getBody() { return body; }
std::map<int, UploadFile>& Request::getUploads() { return (uploads); }

// Setters
void Request::setMethod(std::string& m) { method = m; }
void Request::setPath(std::string& p) { path = p; }
void Request::setVersion(std::string& v) { version = v; }
void Request::setBody(std::string& b) { body = b; }
void Request::addUpload(int fd, UploadFile& new_upload) { uploads[fd] = new_upload; }

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

int Request::parse(std::string buffer) {
    std::stringstream s(buffer);
    std::string line;
    // std::cout <<"Buffer  -> " << buffer << '\n';
    if (std::getline(s, line)) {
        std::vector<std::string> holder;
        holder = split(line, 1, ' ');
        // std::cout << line << " -> " << holder.size() << '\n';
        if (holder.size() != 3)
            return (1);
        setMethod(holder[0]);
        setPath(holder[1]);
        setVersion(holder[2]);
    }
    while (std::getline(s, line) && line != "\r") {
        // std::cout << line << std::endl;
        std::vector<std::string> holder = split(line, 0, ':');
        // std::cout << line << " -> " << holder.size() << '\n';
        if (holder.size() != 2)
            return (1);
        Headers[holder[0]] = holder[1];
        // std::cout << holder[0] << " ==> " << holder[1] << std::endl;
    }
    // print headers
    // std::map<std::string, std::string>::iterator it;
    // for (std::map<std::string, std::string>::iterator it = Headers.begin(); it != Headers.end(); it++) {
    //     std::cout << "{ " << it->first << " = " << it->second
    //         << " }" << std::endl;
    // }
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
                return (1);
            size_t stop_p = str.find("\r\n\r\n");
            if (stop_p != std::string::npos) {
                file.setExpectedBytes(file.getExpectedBytes() - (stop_p + 4));
                str = str.substr(stop_p + 4);
                if (handleFilePart(file, str) != 0)
                    return (1);
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
        return (1);
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
    if (str.compare(0, file.getBoundary().size() + 2, "--" + file.getBoundary()) == 0) {
        if (handleFirstPart(file, str) != 0)
            return (1);
    }
    else if (handleFilePart(file, str) != 0)
        return (1);
    return (0);
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
            if (file.getExpectedBytes() == 0)
                file.setState(true);
            return (1);
        }
        str = str.substr(endPos + 2);
        if (!file.getFd() && !file.openFile())
            return (1);
        if (writeFile(file, str) != 0)
            return (1);
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
                if (file.getExpectedBytes() == 0)
                    file.setState(true);
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
            return (1);
        }
        if (file.getExpectedBytes() == 0) // save the new chunk to handle with next request
            file.setTmpContent(str);
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
        if (writeFirstChunk(file, str) != 0)
            return (1);
    }
    else if (!str.empty()) {
        if (checkChunks(file, str) != 0 || writeFile(file, str) != 0) // write chunk and check if there's other chunks
            return (1);
    }
    return (0);
}

int Request::setupPostBody(int fd, std::string str) {
    contentLength = strToDecimal(Headers["content-length"]);
    if (contentLength < 0)
        return (1);
    std::string toSave = str.substr(0, contentLength);
    contentLength -= toSave.size();
    body += toSave;
    if (contentLength != 0)
        unfinishedReqs[fd] = *this;
    return (0);
}

int Request::continuePostBody(Request& req, std::string str) {
    std::string toSave = str.substr(0, req.contentLength);
    req.contentLength -= toSave.size();
    req.body += toSave;
    if (req.contentLength == 0) {
        
    }
    return (0);
}

void Request::handleFiles(int fd, std::string& str) {
    if (uploads.find(fd) != uploads.end() && uploads[fd].getType() == "multipart") {
        if (readFile(uploads[fd], str) != 0)
            uploads.erase(fd);
    }
    else if (uploads.find(fd) != uploads.end() && uploads[fd].getType() == "chunked") {
        if (readChunkedFile(uploads[fd], str) != 0)
            uploads.erase(fd);
    }
}

void Request::handlePostReq(int fd, std::string& str) {
    if (method == "POST" 
        && Headers.find("content-type") != Headers.end()
        && Headers["content-type"].find("boundary=") != std::string::npos) {
        setupFile(fd);
    }
    else if (method == "POST" 
            && Headers.find("transfer-encoding") != Headers.end()) {
        if (Headers["transfer-encoding"] == "chunked")
            setupChunkedFile(fd);
        else {
            // bad request
        }
    }
    else if (method == "POST"
            && Headers.find("content-length") != Headers.end()) {
        setupPostBody(fd, str);
    }
    else if (method == "POST") {
        // bad request
    }
}

void Request::readHeaders(int fd, std::string& str, Server server) {
    size_t stop_p = str.find("\r\n\r\n");
    if (uploads.find(fd) == uploads.end()
        && unfinishedReqs.find(fd) == unfinishedReqs.end()
        && stop_p != std::string::npos) {
        if (parse(str))
            std::cerr << "Invalid Request" << std::endl;
        str = str.substr(stop_p + 4);
        if (method == "POST"
            && Headers.find("content-length") != Headers.end()
            && strToDecimal(Headers["content-length"]) > server.getClientMaxBodySize()) {
            return;
        }
        handlePostReq(fd, str);
    }
    else if (unfinishedReqs.find(fd) != unfinishedReqs.end())
        continuePostBody(unfinishedReqs[fd], str);
    handleFiles(fd, str);
}

int Request::setupChunkedFile(int fd) {
    UploadFile file;
    
    file.setType("chunked");
    addUpload(fd, file);
    return (0);
}

int Request::setupFile(int fd) {
    UploadFile file;
    if (Headers.find("content-length") != Headers.end()) {
        file.setExpectedBytes(strToDecimal(Headers["content-length"]));
        std::cout << "length -> " << file.getExpectedBytes() << std::endl;
    }
    size_t pos = Headers["content-type"].find("boundary=");
    size_t end = Headers["content-type"].find("\r\n", pos);
    std::string boundary = Headers["content-type"].substr(pos + 9, end);
    file.setType("multipart");
    file.setBoundary(boundary);
    addUpload(fd, file);
    return (0);
}

int Request::readRequest(int fd, Server server) {
    char buff[4096];
    std::string str;
    ssize_t received = recv(fd, buff, sizeof(buff) - 1, 0);
    // std::cout << "received: " << received << std::endl;
    if (received < 0) {
        std::cerr << "Failed to read!" << std::endl;
        close(fd);
        return (1);
    }
    else if (received == 0) {
        std::cout << "Connection closed!" << std::endl;
        if (uploads.find(fd) != uploads.end())
            uploads.erase(fd);
        else if (unfinishedReqs.find(fd) != unfinishedReqs.end())
            unfinishedReqs.erase(fd);
        close(fd);
        return (1);
    }
    else {
        buff[received] = '\0';
        str.append(buff, received);
        // std::cout << "received: " << str << std::endl;
        readHeaders(fd, str, server);
    }
    return (0);
}