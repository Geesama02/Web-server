/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:15:32 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/16 11:30:26 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include "UploadFile.hpp"
#include "../Config/Server.hpp"

class Request {
    private:
        static std::map<int, UploadFile> uploads;
        static std::map<int, Request> unfinishedReqs;
        std::map<std::string, std::string> Headers;
        std::string method;
        long long contentLength;
        std::string path;
        std::string version;
        std::string body;
    public:
        // Constructor
        Request();

        // Getters
        std::map<std::string, std::string>& getHeaders();
        std::string getMethod();
        std::string getPath();
        std::string getVersion();
        std::string getBody();
        std::map<int, UploadFile>& getUploads();

        // Setters
        void setMethod(std::string& m);
        void setPath(std::string& p);
        void setVersion(std::string& v);
        void setBody(std::string& b);
        void addUpload(int fd, UploadFile& new_upload);

        // Functions
        int parse(std::string buffer);
        std::vector<std::string> split(std::string buffer, int full, char del);
        void to_lower(std::string& str);
        int readRequest(int fd, Server server);
        void readHeaders(int fd, std::string& str, Server server);
        int setupFile(int fd);
        int setupChunkedFile(int fd);
        int setupPostBody(int fd, std::string str);
        int continuePostBody(Request& req, std::string str);
        int readFile(UploadFile& file, std::string str);
        int readChunkedFile(UploadFile& file, std::string str);
        long long hexToDecimal(std::string str);
        long long strToDecimal(std::string str);
        int writeFile(UploadFile& file, std::string& str);
        int writeFirstChunk(UploadFile& file, std::string& str);
        int checkChunks(UploadFile& file, std::string& str);
        int handleFilePart(UploadFile& file, std::string& str);
        int handleFirstPart(UploadFile& file, std::string& str);
        void handleFiles(int fd, std::string& str);
        void handlePostReq(int fd, std::string& str);
        
};

#endif