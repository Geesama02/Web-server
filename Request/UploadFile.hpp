/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 10:58:22 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/08 16:03:58 by oait-laa         ###   ########.fr       */
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
        size_t expectedBytes;
        bool firstRead;
        bool state;
        std::string boundary;
    public:
        // Constructor
        UploadFile();

        // Getters
        std::ofstream* getFd();
        std::string getFilename();
        std::string getType();
        bool isFirstRead();
        bool getState();
        std::string getBoundary();

        // Setters
        void setFilename(std::string name);
        void setType(std::string n_type);
        void setState(bool s);
        void setFirstRead(bool s);
        void setBoundary(std::string name);

        // Functions
        int openFile();
        void newFilename();

        // Destructor
        ~UploadFile();
};

#endif