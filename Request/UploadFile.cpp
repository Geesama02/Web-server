/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:07:50 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/19 16:41:29 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UploadFile.hpp"

// Constructor
UploadFile::UploadFile() {
    fd = NULL;
    expectedBytes = 0;
    state = false;
    firstRead = true;
}

// Getters
std::ofstream* UploadFile::getFd() { return (fd); }
std::string UploadFile::getFilename() { return (filename); }
std::string UploadFile::getTmpContent() { return (tmpContent); }
long long UploadFile::getExpectedBytes() { return (expectedBytes); }
std::string UploadFile::getType() { return (type); }
std::string UploadFile::getBoundary() { return (boundary); }
bool UploadFile::isFirstRead() { return (firstRead); }
bool UploadFile::getState() { return (state); }

// Setters
void UploadFile::setFilename(std::string name) { filename = name; }
void UploadFile::setTmpContent(std::string tmp) { tmpContent = tmp; }
void UploadFile::setType(std::string n_type) { type = n_type; }
void UploadFile::setExpectedBytes(long long bytes) { expectedBytes = bytes; }
void UploadFile::setFirstRead(bool fr) { firstRead = fr; }
void UploadFile::setState(bool s) { state = s; }
void UploadFile::setBoundary(std::string name) { boundary = name; }

// Functions
void UploadFile::newFilename() {
    struct timeval tv;
    std::ostringstream time;
    gettimeofday(&tv, NULL);
    time << (tv.tv_sec * 1000000) + tv.tv_usec;
    size_t pointPos = filename.rfind('.');
    if (pointPos != std::string::npos && filename != ".tmp")
        filename.insert(pointPos, "_" + time.str());
    else
        filename.insert(filename.size(), "_" + time.str());
}

int UploadFile::openFile() {
    newFilename();
    filename = "../../goinfre/" + filename;
    std::cout << "file allocated!!" << std::endl;
    fd = new std::ofstream(filename.c_str());
    if (fd->is_open()) {
        // std::cout << "file created for " << filename << std::endl;
        return (1);
    }
    else {
        std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        delete fd;
        fd = NULL;
    }
    return (0);
}

// Destructor
UploadFile::~UploadFile() {
    // std::cout << "Destroyed file -> |" << filename << "|\n";
    if (fd && fd->is_open()) {
        if (!state) { // delete file if connection closed before full file uploaded
            int status = remove(filename.c_str());
            if (status != 0)
                std::cerr << "Error removing file: " << strerror(errno) << std::endl;
        }
        fd->close();
        delete fd;
        fd = NULL;
    }
}