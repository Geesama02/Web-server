/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:55:34 by maglagal          #+#    #+#             */
/*   Updated: 2025/03/14 13:13:49 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <dirent.h>
#include <map>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <fstream>
#include <iomanip>
#include <ftw.h>

#include "../Cgi/Cgi.hpp"

class Config;
class Location;
class Request;

class Response {
    private:
        std::map<std::string, std::string> Headers;
        std::map<int, std::string>         resStatus;
        std::vector<std::string>           savedRedirects;
        std::ifstream                      *file;
        std::ifstream                      *errorPage;
        int                                clientFd;
        int                                statusCode;
        int                                errStatusCode;
        std::string                        filePath;
        std::string                        statusMssg;
        std::string                        finalRes;
        std::string                        queryString;
        std::string                        currentDirAbsolutePath;
        std::string                        body;
        std::string                        locationHeader;
        std::string                        lastModified;
        std::string                        reqResolved;
        Location*                          locationMatch;
        int                                FileType;
        int                                redirectFlag;
        char                               contentLengthHeader[150];
        int                                cgiScript;
        size_t                             bytesToSend;
        size_t                             bytesSent;
        size_t                             nbrRedirections;

    public:
        static std::map<std::string, std::string> ContentTypeHeader;

        //constructor
        Response();
        ~Response();

        //getters
        int                                 getClientFd();
        int                                 getStatusCode();
        std::string                         getQueryString();
        std::string                         getStatusMssg();
        std::map<std::string, std::string>& getHeadersRes( );
        std::string                         getHeader( std::string key );
        std::ifstream*                      getFile();

        //setters
        void            setClientFd(int nFd);
        void            setStatusCode(int value);
        void            setQueryString(std::string value);
        void            setStatusMssg(std::string value);
        void            setHeader( std::string key, std::string value );
        void            setFile(std::ifstream *nFile);
        
        //other
        void                leadingSlashDir(Request& req, std::string& locationIndex, std::string& indexFile);
        void                indexFileWithoutSlashes(Config& config, std::string& indexFile, std::string& pathMatch, std::string& locationIndex, Location* locationIndexMatch);
        void                listOrIndex(Config &config, Request& req, std::string& indexFile);
        void                updateIndexFilePath(Config& config, std::string& indexFile, std::string& locationIndex, std::string& pathMatch);
        void                IndexFileLogic(Config& config, Request& req);
        void                verifyInfiniteRedirections(std::string toCheck);
        void                makeContentRangeHeader(Request& req, std::vector<std::string>& rangeNumbers, std::string& rangeNumber, std::string& contentLengthHeader, size_t& rangeEndNbr);
        void                rangeResponseFail(Config& config, Request& req);
        void                handleCgiScript(Config& config, std::string& fileName);
        void                returnResponse(Config& config);
        static int          callbackRemove(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
        static void         matchReqPathWithLocation(Location& loc, std::string reqPath, Location **match);
        static std::string  getDate();
        static std::string  getDate(time_t *time);
        int                 rmrf(char *path);
        void                handleDeleteRequest(Config& config);
        void                fillBody(Config& config, Request& req);
        void                initializeContentHeader();
        void                checkForFileExtension(std::string fileName);
        void                returnDefinedPage(Config& config, int errorStatus, std::string rootPath, std::string errorPageFile);
        int                 checkDefinedErrorPage(Config& config, std::string rootPath, std::map<int, std::string> error_page);
        void                checkAutoIndex(Config& config, Request& req);
        void                checkErrorPages(Config& config, Request& req);
        void                searchLocationsForMatch(Config& config, Request& req);
        void                listDirectories(Request& req, std::string dirName);
        void                showIndexFile(std::string indexFilePath, Request& req);
        int                 comparingReqWithLocation(std::string locationPath, std::string reqPath);
        void                verifyDirectorySlash(std::string fileName, Request& req);
        void                generateRes(Config& config);
        void                initializeStatusRes();
        void                addHeadersToResponse();
        void                clearResponse();
        void                checkForQueryString(std::string& fileName);
        void                successResponse(Config& config);
        void                redirectionResponse(Request req, Config& config);
        void                rangeResponse(Config& config, Request& req);
        void                sendResponse(Config& config, Request& req, int fd);
        int                 sendBodyBytes(Config& config, int epoll_fd);
        void                searchForFile(Config& config, Request& Req);
        std::string         urlEncode(std::string path);
};

#endif
