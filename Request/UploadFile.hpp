/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 10:58:22 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/25 15:15:13 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADFILE_HPP
#define UPLOADFILE_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <sys/time.h>

class UploadFile {
    private:
        std::ofstream* fd;
        std::string filename;
        std::string type;
        std::string tmpContent;
        long long expectedBytes;
        bool firstRead;
        bool state;
        std::string boundary;
        std::string path;
    public:
        // Constructor
        UploadFile();

        // Getters
        std::ofstream* getFd();
        std::string getFilename();
        std::string getTmpContent();
        long long getExpectedBytes();
        std::string getType();
        bool isFirstRead();
        bool getState();
        std::string getBoundary();

        // Setters
        void setFilename(std::string name);
        void setTmpContent(std::string tmp);
        void setExpectedBytes(long long bytes);
        void setType(std::string n_type);
        void setState(bool s);
        void setFirstRead(bool s);
        void setBoundary(std::string name);
        void setPath(std::string p);

        // Functions
        int openFile();
        void newFilename();

        // Destructor
        ~UploadFile();
};

#endif