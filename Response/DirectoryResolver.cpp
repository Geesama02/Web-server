/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/10 16:49:26 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
//error pages
//if an error is not matched you should use a default
//you should match the error pages in root

//location (list or not)
//1- match request url with location url
//2- concatenate path request with location root
//3- if index.html in the dir = not list
//4- if not index.html is there check autoindex on or off
//5- if not index.html and not autoindex = 404 not found

void    Response::showIndexFile(std::string indexFilePath) {
    std::ifstream indexFile(indexFilePath.c_str());
    std::string buff;
    setStatusCode(200);
    while (std::getline(indexFile, buff))
        body += buff;
}

void Response::listDirectories(std::string locationPath, std::string dirName) {
    std::string dirAsbolute = currentDirAbsolutePath + dirName;
    struct dirent *stDir;
    std::string row;

    setStatusCode(200);
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>h1, p {text-align:left}</style>"
                "</head>"
                "<body>"
                "<h1>Webserv</h1>"
                "<hr></hr>";
    
    while ((stDir = readdir(dir))) {
        std::string direntName = stDir->d_name;
        std::string direntPath = locationPath + "/" + direntName;
        row = "<a href=" + direntPath + ">" + "<p>" + direntName;
        row += "</p></a>";
        lDirectoriesPage += row;
    }

    lDirectoriesPage += "</body>";
    lDirectoriesPage += "</html>";
    
    body = lDirectoriesPage;
    
}

void Response::matchReqPathWithLocation(Location loc, std::string reqPath) {
    struct stat st;
    std::string locationPath = loc.getURI();

    std::cout << "location path " << locationPath << std::endl;
    std::cout << "request path " << reqPath << std::endl;
    
    std::string indexFile = locationPath + "/" + loc.getIndex();
    std::string aIndexFile = currentDirAbsolutePath + indexFile;
    std::cout << "index file" << aIndexFile << std::endl;

    std::cout << "index" << loc.getIndex()<<std::endl;
    int res = stat(aIndexFile.c_str(), &st);
    std::cout << "comparing of 2 strs" <<strcmp(locationPath.c_str(), reqPath.c_str())<< std::endl;
    if (!strcmp(locationPath.c_str(), reqPath.c_str()) ) {
        if (loc.getAutoindex()) {
            if (!res && st.st_mode & S_IFREG)
                showIndexFile(aIndexFile);
            else
                listDirectories(locationPath, reqPath);
        }
        else if (!res && st.st_mode & S_IFREG)    
                showIndexFile(aIndexFile);
        else if (res && errno == ENOENT)
            statusCode = 404;
    }
}

void Response::checkAutoIndex(Config& config, Request req) {
    std::string root;
    std::string uri;
    std::string pathMatch;
    std::vector<Location>::iterator itLocations = config.getClientServer()[clientFd].getLocations().begin();
    while (itLocations != config.getClientServer()[clientFd].getLocations().end()) {
        uri = (*itLocations).getURI();
        root = (*itLocations).getRoot();
        pathMatch = root + uri;
        if (uri.length() > 0 && uri[0] == '/') //dir should have a leading slash
            matchReqPathWithLocation(*itLocations, req.getPath());
        itLocations++;
    }
}

void    Response::checkDefinedPage(Config& config, Request req) {
    checkAutoIndex(config, req);
    std::map<std::vector<int>, std::string>::iterator it = config.getClientServer()[clientFd].getErrorPage().begin();
    while (it != config.getClientServer()[clientFd].getErrorPage().end()) {
        std::vector<int>::const_iterator it2 = (it->first).begin();
        while (it2 != (it->first).end()) {
            if (*it2 == statusCode)
                return returnDefinedPage(it->second);
            it2++;
        }
        it++;
    }
}

void    Response::returnDefinedPage(std::string fileName) {
    std::string buffer;
    std::string absolutePath = currentDirAbsolutePath + "/" + fileName;
    std::ifstream definedPage(absolutePath.c_str());
    if (!definedPage.is_open()) {
        std::cerr << "Failed to open defined file " << absolutePath << std::endl;
        return ;
    }
    while (std::getline(definedPage, buffer))
        body += buffer;
}

