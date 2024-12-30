/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:07:04 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/30 17:11:10 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// Getters
std::string Request::getMethod() { return method; }
std::string Request::getPath() { return path; }
std::string Request::getVersion() { return version; }
std::string Request::getHost() { return host; }
std::string Request::getConnection() { return connection; }
std::string Request::getContentLength() { return content_length; }
std::string Request::getContentType() { return content_type; }
std::string Request::getAccept() { return accept; }
std::string Request::getBody() { return body; }

// Setters
void Request::setMethod(const std::string& m) { method = m; }
void Request::setPath(const std::string& p) { path = p; }
void Request::setVersion(const std::string& v) { version = v; }
void Request::setHost(const std::string& h) { host = h; }
void Request::setConnection(const std::string& c) { connection = c; }
void Request::setContentLength(const std::string& cl) { content_length = cl; }
void Request::setContentType(const std::string& ct) { content_type = ct; }
void Request::setAccept(const std::string& a) { accept = a; }
void Request::setBody(const std::string& b) { body = b; }

// Functions
std::vector<std::string> Request::split(std::string buffer, int full, char del) {
    std::stringstream s(buffer);
    std::vector<std::string> holder;
    std::string tmp_str;
    if (full) {
        while (std::getline(s, tmp_str, del)) {
            if (!tmp_str.empty()) {
                holder.push_back(tmp_str);
            }
        }
    }
    else {
        if (std::getline(s, tmp_str, del)) {
            if (*tmp_str.begin() == ' ')
                tmp_str.erase(0, 1);
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

int Request::parse(std::string buffer) {
    std::stringstream s(buffer);
    std::string line;
    if (std::getline(s, line)) {
        std::vector<std::string> holder;
        holder = split(line, 1, ' ');
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
    std::map<std::string, std::string>::iterator it;
    for (std::map<std::string, std::string>::iterator it = Headers.begin(); it != Headers.end(); it++) {
        std::cout << " {" << it->first << ", " << it->second
            << "} " << std::endl;
    }
    // std::cout << "inside\n";
    return (0);
}