/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:07:04 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/08 16:53:35 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

std::map<int, UploadFile> Request::uploads;

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

int Request::readFile(UploadFile& file, std::string str) {
    if (str.compare(0, file.getBoundary().size() + 2, "--" + file.getBoundary()) == 0) {
        size_t namePos = str.find("filename=\"");
        if (namePos != std::string::npos) {
            size_t nameStart = namePos + 10;
            size_t nameEnd = str.find("\"", nameStart);
            if (nameEnd != std::string::npos) {
                file.setFilename(str.substr(nameStart, nameEnd - nameStart));
                // std::cout << "filename -> |" << file.getFilename() <<'|' << std::endl;
                if (file.getFilename().empty())
                    return (1);
                if (!file.openFile())
                    return (1);
                size_t stop_p = str.find("\r\n\r\n");
                if (stop_p != std::string::npos) {
                    str = str.substr(stop_p + 4);
                    *file.getFd() << str;
                }
            }
        }
    }
    else {
        size_t stopPos;
        // std::cout << "is_open -> |" << file.getFd() <<'|' << std::endl;
        // std::cout << "is_open -> |" << file.getFd()->is_open() <<'|' << std::endl;
        if ((stopPos = str.find("\r\n--" + file.getBoundary() + "--")) != std::string::npos) {
            str.erase(stopPos);
            *file.getFd() << str;
            file.setState(true);
            // std::cout << "FOUND\n";
            return (1);
        }
        if (!str.empty())
            *file.getFd() << str;
    }
    return (0);
}

int Request::hexToInt(std::string str) {
    std::stringstream s(str);
    int holder;
    
    s >> std::hex >> holder;
    return (holder);
}
int Request::readChunkedFile(UploadFile& file, std::string str) {
    if (file.isFirstRead() && !str.empty()) {
        file.setFirstRead(false);
        size_t endPos = str.find("\r\n");
        if (endPos != std::string::npos) {
            std::string toRead = str.substr(0, endPos);
            std::cout << "Hex num -> |" << toRead << '|' << std::endl;
            // std::cout << "decimal num -> |" << hexToInt(toRead) << '|' << std::endl;
        }
        else
            std::cout << "not found\n";
    }
    else {
        size_t startPos = str.find("\r\n");
        if (startPos != std::string::npos) {
            startPos = startPos + 2;
            size_t endPos = str.find("\r\n", startPos);
            if (endPos != std::string::npos) {
                std::string toRead = str.substr(startPos, endPos - startPos);
                std::cout << "found\n";
                std::cout << "Hex num -> |" << toRead << '|'<< hexToInt(toRead)<< '|' << std::endl;
            }
        }
    }
    return (0);
}

void Request::readHeaders(int fd, std::string& str) {
    size_t stop_p = str.find("\r\n\r\n");
    // std::cout << "str -> " << str << std::endl;
    if (uploads.find(fd) == uploads.end()
            && stop_p != std::string::npos) {
        if (parse(str))
            std::cerr << "Invalid Request" << std::endl;
        str = str.substr(stop_p + 4);
        // std::cout << "length -> " << Headers["content-length"] << std::endl;
        if (method == "POST" 
            && Headers.find("content-type") != Headers.end()
            && Headers["content-type"].find("boundary=") != std::string::npos) {
            setupFile(fd);
        }
        else if (method == "POST" 
            && Headers.find("transfer-encoding") != Headers.end()) {
                if (Headers["transfer-encoding"] == "chunked") {
                    setupChunkedFile(fd);
                }
                else {
                    // bad request
                }
        }
    }
    if (uploads.find(fd) != uploads.end() && uploads[fd].getType() == "multipart") {
        if (readFile(uploads[fd], str)) {
            uploads.erase(fd);
        }
    }
    else if (uploads.find(fd) != uploads.end() && uploads[fd].getType() == "chunked") {
        if (readChunkedFile(uploads[fd], str)) {
            uploads.erase(fd);
        }
    }
}

int Request::setupChunkedFile(int fd) {
    UploadFile file;
    
    file.setType("chunked");
    addUpload(fd, file);
    return (0);
}

int Request::setupFile(int fd) {
    UploadFile file;
    size_t pos = Headers["content-type"].find("boundary=");
    size_t end = Headers["content-type"].find("\r\n", pos);
    std::string boundary = Headers["content-type"].substr(pos + 9, end);
    file.setType("multipart");
    file.setBoundary(boundary);
    addUpload(fd, file);
    return (0);
}

int Request::readRequest(int fd) {
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
        if (uploads.find(fd) != uploads.end()) {
            std::cout << "Closed"<< std::endl;
            uploads.erase(fd);
        }
        close(fd);
    }
    else {
        buff[received] = '\0';
        str.append(buff, received);
        // std::cout << "received: " << str << std::endl;
        readHeaders(fd, str);
        // if (method == "")
        //     exit(0);
        // if (method == "POST"
        //     && Headers.find("content-length") != Headers.end()) {
        //     // std::cout << "POST str -> |" << str<< "|\n";
        //     size_t s = atoi(Headers["content-length"].c_str());
        //     if (str.size() >= s) {
        //         setBody(str);
        //     }
        // }
    }
    return (0);
}