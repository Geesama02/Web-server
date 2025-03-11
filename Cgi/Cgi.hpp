/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:52 by maglagal          #+#    #+#             */
/*   Updated: 2025/03/10 17:45:47 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>

// #include "../Response/Response.hpp"
#include "../Request/Request.hpp"

class Config;
class Response;
class CGI {
    private :
        int                                  outFileFd;
        std::string                          outFileName;
        std::string                          outFilePath;
        std::map<std::string, std::string>   executablePaths;
        std::map<std::string, std::string>   storeEnvs;
        std::vector<char *>                  Envs;
        std::vector<std::string>             headersInScript;
        char                                 **envs;
        char                                 *argv[3];
        std::string                          scriptRelativePath;
        std::string                          scriptFilePath;
        std::string                          scriptFileName;
        std::string                          extensionFile;
        std::string                          cgiRes;
        std::string                          ResBody;
        std::string                          pathInfo;
        pid_t                                cPid;
        int                                  rPipe;
        int                                  childStatus;
        long long                            startTime;
        char*                                executablePathArray;
        char*                                absoluteFilePath;
        int                                  envsNbr;
        

    public :

        //constructor && destructor
        CGI();
        ~CGI();

        //getters
        std::string     getBody();
        int             getChildStatus();
        pid_t           getCpid();
        int             getRpipe();
        long long       getStartTime();
        std::string&    getPathInfo();
        std::string&    getscriptFilePath();
        std::string&    getExtensionFile();
        int             getOutFileFd();
        std::string&    getOutFileName();

        //setters
        void            setBody(std::string newBody);
        void            setChildStatus(int newChildStatus);
        void            setCpid(pid_t nPid);
        void            setRpipe(int nRpipe);
        void            setStartTime(long long nTime);
        void            setPathInfo(std::string nValue);
        void            setscriptFilePath(std::string nValue);
        void            setExtensionFile(std::string nValue);
        void            setOutFileFd(int nFd);
        void            setOutFileName(std::string& nValue);
        

        //other
        void    generateFileName();
        int     creatingOutFile();
        void    searchForScriptName(std::string& reqPath);
        void    checkHeaderName(std::string& headerName);
        int     failureHandler(Config& config, int fd);
        int     defineArgv(Config& config, int fd);
        void    defineExecutionPaths(int fd, Config& config);
        void    clearCGI();
        int     execute_cgi_script(Config& config, Response& res, int fd, Request req);
        int     setEnvVars(Config& config, Request& req, Response& res, int fd);
        int     findExecutablePath(Config& config, int fd);
        int     read_cgi_response(Config& config, int fd);
        void    sendServerResponse(int fd, Config& config);
        void    findHeadersInsideScript(Response& res);
        void    convertHeaderToCamelCase(std::string& value);
        void    defineResponseStatusMssg(Response& res);
        int     addMetaVariables(Config& config, Request& req, Response& res);
};

#endif
