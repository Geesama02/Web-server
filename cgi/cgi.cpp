/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:44 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/06 11:28:01 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CGI::CGI() {
    executablePaths[".py"] = "/usr/bin/python3";
    executablePaths[".php"] = "/usr/bin/php";
}

CGI::~CGI() {
    for(int i = 0; i < 6; i++)
        delete[] envs[i];
    delete[] executablePathArray;
    delete[] absoluteFilePath;
}

void CGI::convertHeaderToCamelCase(std::string& value) {
    if (value[0] >= 97 && value[0] <= 122) {
        size_t index = value.find('-');
        value[0] = value[0] - 32;
        value[index + 1] = value[index + 1] - 32;
    }
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

void CGI::findHeadersInsideScript(Request req, Response& res) {
    int contentLengthFlag = 0;

    std::string headerInScript = ResBody.substr(0, ResBody.find("\n\n"));
    ResBody.erase(0, ResBody.find("\n\n"));
    if (headerInScript.find(":") != std::string::npos) {
        headersInScript = req.split(headerInScript, 0, '\n');
        std::vector<std::string>::iterator it = headersInScript.begin();
        while (it != headersInScript.end()) {
            std::string headerName = (*it).substr(0, (*it).find(':'));
            std::string headerValue = (*it).substr((*it).find(' ') + 1);
            convertHeaderToCamelCase(headerName);
            std::cout << "header name " << headerName << std::endl;
            if (headerName == "Content-Length")
                contentLengthFlag = 1;
            // std::cout << "|" << headerName << "|" << std::endl;
            res.setHeader(headerName, headerValue);
            it++;
        }
    }

    //define content length
    if (!contentLengthFlag) {
        char buff[150];
        sprintf(buff, "%ld", ResBody.length());
        res.setHeader("Content-Length", buff);
    }
}

void CGI::sendServerResponse(int fd, Response& res, Request req)
{
    cgiRes += res.getStatusMssg();

    //check if there is already content-type defined
    findHeadersInsideScript(req, res);

    std::map<std::string, std::string>::iterator it = res.getHeadersRes().begin();
    while (it != res.getHeadersRes().end()) {
        std::string header = it->first + ": " + it->second;
        std::cout << "header " << header << std::endl;
        cgiRes += header + "\r\n";
        it++;
    }

    if (ResBody.length() > 0)
        cgiRes += ResBody;

    send(fd, cgiRes.c_str(), cgiRes.length(), 0);
} 

void CGI::execute_cgi_script(Response& res, int fd, Request req)
{
    initializeVars(req);

    //set environment variables 
    setEnvVars(req);

    //find the absolute path of the script
    findExecutablePath();

    int save_out = dup(1);
    int fds[2];
    if (pipe(fds) != 0)
        std::cerr << "pipe failed!!" << std::endl;
    int c_fd = fork();
    if (c_fd == 0) {
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);
        if (execve(executablePathArray, argv, envs) ==-1) {
            std::cerr << "execve failed!!";
            std::cerr << strerror(errno) << std::endl;
            return ;
        }
        dup2(save_out, 1);
        close(save_out);
    }
    close(fds[1]);
    close(c_fd);
    close(save_out);
    waitpid(-1, NULL, 0);

    // read the ouput of the script executed by the cgi
    read_cgi_response(fds[0]);
    close(fds[0]);
    sendServerResponse(fd, res, req);
}
