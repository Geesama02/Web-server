/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:15:32 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/10 14:38:05 by oait-laa         ###   ########.fr       */
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

class Request {
    private:
        static std::map<int, UploadFile> uploads;
        std::map<std::string, std::string> Headers;
        std::string method;
        std::string path;
        std::string version;
        std::string body;
    public:
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
        int readRequest(int fd);
        void readHeaders(int fd, std::string& str);
        int setupFile(int fd);
        int setupChunkedFile(int fd);
        int readFile(UploadFile& file, std::string str);
        int readChunkedFile(UploadFile& file, std::string str);
        int hexToInt(std::string str);
        int writeFile(UploadFile& file, std::string& str);
        int writeFirstChunk(UploadFile& file, std::string& str);
        int checkChunks(UploadFile& file, std::string& str);
        
};

#endif