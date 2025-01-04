/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 10:58:22 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/04 16:11:51 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADFILE_HPP
#define UPLOADFILE_HPP

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>

class UploadFile {
    private:
        std::ofstream* fd;
        std::string filename;
        bool state;
        std::string boundary;
    public:
        // Constructor
        UploadFile();

        // Getters
        std::ofstream* getFd();
        std::string getFilename();
        bool getState();
        std::string getBoundary();

        // Setters
        // void setFd(std::ofstream f);
        void setFilename(std::string name);
        void setState(bool s);
        void setBoundary(std::string name);

        // Functions
        int openFile();

        // Destructor
        ~UploadFile();
};

#endif