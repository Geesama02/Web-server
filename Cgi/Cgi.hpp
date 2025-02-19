/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:52 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/17 12:05:26 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

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

#include "./Child.hpp"
#include "../Response/Response.hpp"
#include "../Request/Request.hpp"


class CGI {
    private :
        std::vector<char *> Envs;
        std::map<std::string, std::string>   executablePaths;
        std::vector<std::string>             headersInScript;
        char *envs[8];
        char *argv[3];
        std::string scriptRelativePath;
        std::string scriptFileName;
        std::string extensionFile;
        std::string cgiRes;
        std::string ResBody;
        pid_t       cPid;
        int         rPipe;
        int         timeout;
        long long   startTime;
        char* executablePathArray;
        char* absoluteFilePath;
        

    public :

        //constructor && destructor
        CGI();
        ~CGI();
        // CGI( const CGI& obj );
        // CGI& operator=( const CGI& obj );

        //getters
        pid_t     getCpid() const;
        int       getRpipe();
        int       getTimeout();
        long long getStartTime();

        //setters
        void   setCpid(pid_t nPid);
        void   setRpipe(int nRpipe);
        void   setTimeout(int nTimeout);
        void   setStartTime(long long nTime);
        

        //other
        void execute_cgi_script(Config& config, Response& res, int fd, Request req);
        void initializeVars(Response& res, Request req);
        void setEnvVars(Request req, Response& res);
        void findExecutablePath();
        void read_cgi_response();
        void sendServerResponse(int fd, Response& res);
        void findHeadersInsideScript(Response& res);
        void convertHeaderToCamelCase(std::string& value);
        void defineResponseStatusMssg(Response& res);
};

#endif