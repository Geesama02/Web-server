/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:07:50 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/04 16:10:19 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UploadFile.hpp"

// Constructor
UploadFile::UploadFile() {
    fd = NULL;
    state = false;
}

// Getters
std::ofstream* UploadFile::getFd() { return (fd); }
std::string UploadFile::getFilename() { return (filename); }
std::string UploadFile::getBoundary() { return (boundary); }
bool UploadFile::getState() { return (state); }

// Setters
// void UploadFile::setFd(std::ofstream f) { fd = f; }
void UploadFile::setFilename(std::string name) { filename = name; }
void UploadFile::setState(bool s) { state = s; }
void UploadFile::setBoundary(std::string name) { boundary = name; }

// Functions
int UploadFile::openFile() {
    fd = new std::ofstream(filename.c_str());
    // fd->open(filename.c_str());
    if (fd->is_open())
        return (1);
    else {
        std::cerr << strerror(errno) << std::endl;
        delete fd;
    }
    return (0);
}

// Destructor
UploadFile::~UploadFile() {
    if (fd) {
        fd->close();
        delete fd;
    }
}