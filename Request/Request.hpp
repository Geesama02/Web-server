/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:15:32 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/17 16:19:06 by oait-laa         ###   ########.fr       */
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
#include <ctime>
#include "UploadFile.hpp"
// #include "../Config/Config.hpp"

class Response;
class Server;

class Request {
    private:
        std::map<std::string, std::string> Headers;
        static std::map<int, std::string>  reqStatus;
        bool                               isUploading;
        UploadFile                         *file;
        std::string                        fileName;
        std::string                        method;
        long long                          contentLength;
        std::string                        path;
        std::string                        version;
        std::string                        body;
    public:
        // static std::map<int, UploadFile>   uploads;
        // static std::map<int, Request>      unfinishedReqs;

        // Constructor
        Request();

        // Getters
        std::map<std::string, std::string>& getHeaders();
        std::string                         getMethod();
        std::string                         getFileName();
        std::string                         getPath();
        std::string                         getVersion();
        std::string                         getBody();
        // std::map<int, UploadFile>&          getUploads();

        // Setters
        void setMethod(std::string& m);
        void setPath(std::string& p);
        void setVersion(std::string& v);
        void setBody(std::string& b);
        void addUpload(UploadFile& new_upload);

        // Functions
        int parse(std::string buffer, size_t stop_p);
        int isNumber(std::string& str);
        std::vector<std::string> split(std::string buffer, int full, char del);
        void to_lower(std::string& str);
        int readRequest(int fd, Server& server, std::vector<Server>& Servers);
        int readHeaders(std::string& str, Server& server, std::vector<Server>& Servers);
        int setupFile();
        int setupChunkedFile();
        int setupPostBody();
        int continuePostBody(std::string str);
        int readFile(UploadFile& file, std::string str);
        int readChunkedFile(UploadFile& file, std::string str);
        int readBinaryFile(UploadFile& file, std::string str);
        long long hexToDecimal(std::string str);
        long long strToDecimal(std::string str);
        int writeFile(UploadFile& file, std::string& str);
        int writeFirstChunk(UploadFile& file, std::string& str);
        int checkChunks(UploadFile& file, std::string& str);
        int handleFilePart(UploadFile& file, std::string& str);
        int handleFirstPart(UploadFile& file, std::string& str);
        int handleFiles(std::string& str);
        int handlePostReq();
        Server getServer(Server& server, std::vector<Server>& Servers);
        int setupBinaryFile();
        std::string getExtension(std::string type);
        std::string generateRes(int status);
        std::string getDate();
        int checkMethod(std::string str);

        // Destructor
        ~Request();
};

#endif