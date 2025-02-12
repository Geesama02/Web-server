/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/12 14:29:23 by oait-laa         ###   ########.fr       */
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

int    Response::comparingReqWithLocation(std::string locationPath, std::string reqPath) {
    std::cout << locationPath << std::endl;
    std::cout << reqPath << std::endl;
    size_t i = locationPath.find(reqPath);
    std::string rest = locationPath.substr(i);
    return (0);
}

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
    struct stat st;
    std::string row;
    std::string direntName;
    std::string direntPath;

    setStatusCode(200);
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>p {text-align:left;font-size: small} h1 {text-align: left; font-size: large}</style>"
                "</head>"
                "<body>"
                "<h1>Webserv</h1>"
                "<hr></hr>";
    
    while ((stDir = readdir(dir))) {
        direntName = stDir->d_name;
        direntPath = locationPath + direntName;
        std::string absoluteDirentPath = currentDirAbsolutePath + direntPath;
        stat(absoluteDirentPath.c_str(), &st);
        if (st.st_mode & S_IFDIR) {
            direntPath += "/";
            direntName += "/";
        }
        row = "<a href=" + direntPath + ">" + "<p>" + direntName;
        row += "</p></a>";
        lDirectoriesPage += row;
    }

    lDirectoriesPage += "</body>";
    lDirectoriesPage += "</html>";
    
    body = lDirectoriesPage;
}

void Response::matchReqPathWithLocation(Location loc, std::string reqPath, std::string toMatch) {
    struct stat st;
    struct stat reqPathCheck;
    std::string locationPath = loc.getURI();    

    std::cout << "location path " << toMatch << std::endl;
    std::cout << "request path " << reqPath << std::endl;
    
    std::string indexFile = reqPath + loc.getIndex();
    std::string aIndexFile = currentDirAbsolutePath + indexFile;

    std::cout << "index file " << aIndexFile << std::endl;
    std::cout << "index " << loc.getIndex() << std::endl;
    std::string reqPathAsbsolute = currentDirAbsolutePath + reqPath;
    stat(reqPathAsbsolute.c_str(), &reqPathCheck);
    int res = stat(aIndexFile.c_str(), &st);

    size_t pos = toMatch.rfind("/");
    std::cout << "comparing of 2 strs" <<strncmp(toMatch.c_str(), reqPath.c_str(), pos + 1)<< std::endl;

    // comparingReqWithLocation(locationPath, reqPath);
    if (!strncmp(toMatch.c_str(), reqPath.c_str(), pos + 1) && (reqPathCheck.st_mode & S_IFDIR)) {
        if (loc.getAutoindex()) {
            if (!res && st.st_mode & S_IFREG)
                showIndexFile(aIndexFile);
            else
                listDirectories(locationPath, reqPath);
        }
        else if (!res && st.st_mode & S_IFREG)
            showIndexFile(aIndexFile);
    }
}

void Response::checkAutoIndex(Config& config, Request req) {
    std::string root;
    std::string uri;
    std::string nUri;
    std::string pathMatch;
    std::vector<Location>::iterator itLocations = config.getClientServer()[clientFd].getLocations().begin();

    while (itLocations != config.getClientServer()[clientFd].getLocations().end()) {
        uri = (*itLocations).getURI();
        if (uri.rfind("/") == 0 && uri.length() != 1) {
            uri = uri + "/";
            (*itLocations).setURI(uri);
        }
        root = (*itLocations).getRoot();
        if (root != "/")
            pathMatch = root + uri;
        else
            pathMatch = uri;
        if (uri.length() > 0 && uri[0] == '/') //dir should have a leading slash
            matchReqPathWithLocation(*itLocations, req.getPath(), pathMatch);
        itLocations++;
    }
}

void    Response::checkDefinedPage(Config& config, Request req) {
    checkAutoIndex(config, req);
    // std::map<std::vector<int>, std::string>::iterator it = config.getClientServer()[clientFd].getErrorPage().begin();
    // while (it != config.getClientServer()[clientFd].getErrorPage().end()) {
    //     std::vector<int>::const_iterator it2 = (it->first).begin();
    //     while (it2 != (it->first).end()) {
    //         if (*it2 == statusCode)
    //             return returnDefinedPage(it->second);
    //         it2++;
    //     }
    //     it++;
    // }
}

// void    Response::returnDefinedPage(std::string fileName) {
//     std::string buffer;
//     std::string absolutePath = currentDirAbsolutePath + "/" + fileName;
//     std::ifstream definedPage(absolutePath.c_str());
//     if (!definedPage.is_open()) {
//         std::cerr << "Failed to open defined file " << absolutePath << std::endl;
//         return ;
//     }
//     while (std::getline(definedPage, buffer))
//         body += buffer;
// }

