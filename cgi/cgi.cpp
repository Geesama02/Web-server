/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:44 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/05 09:53:11 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CGI::CGI() {
    executablePaths[".py"] = "/usr/bin/python3";
    executablePaths[".php"] = "/usr/bin/php";
}

CGI::~CGI() {
    for(int i = 0; i < 6; i++)
        delete envs[i];
}

void CGI::initializeVars(Request req)
{
    scriptRelativePath = req.getPath().empty() ? "/" : req.getPath().c_str();
}

void CGI::setEnvVars(Request req)
{
    char contentLengthStr[150];
    if (req.getBody().length() > 0)
        sprintf(contentLengthStr, "%ld", req.getBody().length());
    std::map<std::string, std::string>storeEnvs;
    storeEnvs["REQUEST_METHOD"] = req.getMethod().c_str();
    storeEnvs["SCRIPT_NAME"] = req.getPath().empty() ? "/" : req.getPath().c_str();
    storeEnvs["SERVER_NAME"] = "Webserv";
    storeEnvs["SERVER_PROTOCOL"] = "HTTP 1.1";
    storeEnvs["CONTENT_LENGTH"] = req.getBody().length() > 0 ? contentLengthStr : "";
    storeEnvs["CONTENT_TYPE"] = req.getHeaders()["content-type"].c_str();

    std::map<std::string, std::string>::iterator it = storeEnvs.begin();
    for (int i = 0; i < 6; i++) {
        std::string env = it->first + "=" + it->second;
        envs[i] = new char[env.length() + 1];
        std::strcpy(envs[i], env.c_str());
        it++;
    }
    envs[6] = NULL;
}

void CGI::findExecutablePath() 
{
    int index = scriptRelativePath.rfind(".");
    int index2 = scriptRelativePath.rfind("/");
    std::string tmp = scriptRelativePath;
    extensionFile = tmp.erase(0, index);
    tmp = scriptRelativePath;
    scriptFileName = tmp.erase(0, index2);
    scriptFileName.erase(0, 1);
    std::map<std::string, std::string>::iterator it = executablePaths.begin();
    while (it->first != extensionFile)
        it++;
    std::string executablePath = it->second;
    char buff[120];
    getcwd(buff, 120);
    std::string current_dir = buff;
    std::string absolutePath = current_dir + scriptRelativePath;
    executablePathArray = new char[executablePath.length() + 1];
    absoluteFilePath = new char[absolutePath.length() + 1];
    std::strcpy(executablePathArray, executablePath.c_str());
    std::strcpy(absoluteFilePath, absolutePath.c_str());
    argv[0] = executablePathArray;
    argv[1] = absoluteFilePath;
    argv[2] = NULL;
}

void CGI::read_cgi_response(int fd_r)
{
    char buff2[1025];
    size_t nbytes = read(fd_r, buff2, 1024);
    buff2[nbytes] = '\0';
    ResBody += buff2;
    while ((nbytes = read(fd_r, buff2, 1024)) > 0) {
        buff2[nbytes] = '\0';
        ResBody += buff2;
    }
}

void CGI::sendServerResponse(int fd, Response res)
{
    cgiRes += res.getStatusMssg();
    std::map<std::string, std::string>::iterator it = res.getHeadersRes().begin();
    while (it != res.getHeadersRes().end()) {
        std::cout << it->first << std::endl;
        std::cout << it->second << std::endl;
        std::string header = it->first + ": " + it->second;
        cgiRes += header + "\r\n";
        it++;
    }
    // cgiRes += "\r\n";
    if (ResBody.length() > 0)
        cgiRes += ResBody;
    
    char buff[120];
    sprintf(buff, "%ld", cgiRes.length());
    res.setHeader("Content-Length", buff);
    // (void)fd;
    std::cout << "cgi body " << ResBody << std::endl;
    std::cout << "cgi res " << cgiRes << std::endl;
    send(fd, cgiRes.c_str(), cgiRes.length(), 0);
} 

void CGI::execute_cgi_script(Response res, int fd, Request req, char **envp)
{
    (void)envp;
    (void)fd;
    initializeVars(req);
    int save_out = dup(1);
    int fds[2];
    if (pipe(fds) != 0)
        std::cerr << "pipe failed!!" << std::endl;
    int c_fd = fork();
    if (c_fd == 0) {
        //set environment variables 
        setEnvVars(req);

        //find the absolute path of the script
        findExecutablePath();

        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);
        if (execve(executablePathArray, argv, envs) ==-1)
            std::cerr << strerror(errno) << std::endl;
        // dup2(save_out, 1);
        close(save_out);
    }
    close(fds[1]);
    close(c_fd);
    close(save_out);
    waitpid(-1, NULL, 0);

    // read the ouput of the script executed by the cgi
    read_cgi_response(fds[0]);
    close(fds[0]);
    sendServerResponse(fd, res);
}