/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:52 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/04 17:55:19 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
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

#include "../Response/Response.hpp"

class Response;
class CGI {
    private :
        std::map<std::string, std::string> executablePaths;
        char *envs[7];
        char *argv[3];
        std::string scriptRelativePath;
        std::string scriptFileName;
        std::string extensionFile;
        std::string cgiRes;
        std::string ResBody;
        char*       executablePathArray;
        char*       absoluteFilePath;
    public :
        CGI();
        ~CGI();
        void execute_cgi_script(Response res, int fd, Request req, char **envp);
        void initializeVars(Request req);
        void setEnvVars(Request req);
        void findExecutablePath();
        void read_cgi_response(int fd_r);
        void sendServerResponse(int fd, Response res);
};

#endif