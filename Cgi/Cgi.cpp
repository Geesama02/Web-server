/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:44 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/20 10:06:12 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "../Config/Config.hpp"

CGI::CGI() {
    for(int i = 0; i < 7; i++)
        envs[i] = NULL;
    executablePathArray = NULL;
    absoluteFilePath = NULL;
    timeout = 5000;
    executablePaths[".py"] = "/home/maglagal/Desktop/webserv/cgi-bin/myenv/bin/python";
    executablePaths[".php"] = "/usr/bin/php";
}

CGI::~CGI() {
    for(int i = 0; i < 7; i++) {
        if (envs[i])
            delete[] envs[i];
    }
    if (absoluteFilePath)
        delete[] absoluteFilePath;
    if (executablePathArray)
        delete[] executablePathArray;
}

//getters
pid_t     CGI::getCpid() const { return cPid; }
int       CGI::getRpipe() { return rPipe; }
pid_t     CGI::getTimeout() { return timeout; }
long long CGI::getStartTime() { return startTime; }

//setters
void    CGI::setCpid(pid_t nPid) { cPid = nPid; }
void    CGI::setRpipe(int nRpipe) { rPipe = nRpipe; }
void    CGI::setTimeout(int nTimeout) { timeout = nTimeout; }
void    CGI::setStartTime(long long nTime) {startTime = nTime;}


void CGI::defineResponseStatusMssg(Response& res) {  
    if (res.getStatusCode() == 200)
        res.setStatusMssg("HTTP/1.1 200 OK\n");
    else if (res.getStatusCode() == 404)
        res.setStatusMssg("HTTP/1.1 404 Not Found\n");
}

void CGI::convertHeaderToCamelCase(std::string& value) {
    if (value[0] >= 97 && value[0] <= 122) {
        size_t index = value.find('-');
        value[0] = value[0] - 32;
        value[index + 1] = value[index + 1] - 32;
    }
}

void CGI::initializeVars(Response& res, Request req)
{
    std::string reqPath = req.getPath();
    if (!reqPath.empty()) {
        res.checkForQueryString(reqPath);
        scriptRelativePath = reqPath.c_str();
    }
    else
        scriptRelativePath = "/";
}

void CGI::setEnvVars(Request req, Response& res)
{
    char contentLengthStr[150];
    if (req.getBody().length() > 0)
        sprintf(contentLengthStr, "%ld", req.getBody().length());

    std::map<std::string, std::string> storeEnvs;
    storeEnvs["REQUEST_METHOD"] = req.getMethod().c_str();
    storeEnvs["SCRIPT_NAME"] = req.getPath().empty() ? "/" : req.getPath().c_str();//forbidden!!
    storeEnvs["SERVER_NAME"] = "Webserv";
    storeEnvs["SERVER_PROTOCOL"] = "HTTP 1.1";
    storeEnvs["CONTENT_LENGTH"] = req.getBody().length() > 0 ? contentLengthStr : ""; //forbidden!!
    storeEnvs["CONTENT_TYPE"] = req.getHeaders()["content-type"].c_str();
    storeEnvs["QUERY_STRING"] = (res.getQueryString()).c_str();
    
    std::map<std::string, std::string>::iterator it = storeEnvs.begin();
    for (int i = 0; i < 7; i++) {
        std::string env = it->first + "=" + it->second;
        envs[i] = new char[env.length() + 1];
        std::strcpy(envs[i], env.c_str());
        it++;
    }
    envs[7] = NULL;
}

void CGI::findExecutablePath() 
{
    size_t index = scriptRelativePath.rfind("."); //you need some error handling here
    size_t index2 = scriptRelativePath.rfind("/"); //you need some error handling here
    std::string tmp = scriptRelativePath;
    if (index != std::string::npos)
        extensionFile = tmp.erase(0, index);
    tmp = scriptRelativePath;
    if (index2 != std::string::npos)
        scriptFileName = tmp.erase(0, index2);
    scriptFileName.erase(0, 1);
    std::map<std::string, std::string>::iterator it = executablePaths.begin();
    while (it->first != extensionFile)
        it++;
    if (it == executablePaths.end()) {
        std::cerr << "Extension file not found" << std::endl;
        exit(1);
    }
    std::string executablePath = it->second;
    char buff[120];
    getcwd(buff, 120);
    std::string current_dir = buff;
    std::string absolutePath = current_dir + scriptRelativePath;
    // executablePathArray = new char[executablePath.length() + 1];
    // absoluteFilePath = new char[absolutePath.length() + 1];
    executablePathArray = new char[executablePath.length() + 1];
    absoluteFilePath = new char[absolutePath.length() + 1];
    std::strcpy(executablePathArray, executablePath.c_str());
    std::strcpy(absoluteFilePath, absolutePath.c_str());
    argv[0] = executablePathArray;
    argv[1] = absoluteFilePath;
    argv[2] = NULL;
}


void CGI::findHeadersInsideScript(Response& res) {
    int contentLengthFlag = 0;
    int contentTypeFlag = 0;
    std::string headerInScript;
    std::string headerName;
    std::string headerValue;

    size_t i = ResBody.find("\n\n");
    if (i != std::string::npos) {
        headerInScript = ResBody.substr(0, i);
        ResBody.erase(0, i + 1);
    }
    if (headerInScript.length() > 0) {
        if (headerInScript.find(":") != std::string::npos) {
            headersInScript = Request::split(headerInScript, 0, '\n');
            std::vector<std::string>::iterator it = headersInScript.begin();
            while (it != headersInScript.end()) {
                headerName = (*it).substr(0, (*it).find(':'));
                headerValue = (*it).substr((*it).find(' ') + 1);
                convertHeaderToCamelCase(headerName);
                if (headerName == "Content-Length")
                    contentLengthFlag = 1;
                if (headerName == "Content-Type")
                    contentTypeFlag = 1;
                res.setHeader(headerName, headerValue);
                it++;
            }
        }
    }

    //remove leading empty newlines
    while(ResBody[0] == '\n')
        ResBody.erase(0, 1);

    //define content length and content-type in case not defined in the script
    if (!contentLengthFlag) {
        char buff[150];
        sprintf(buff, "%ld", ResBody.length());
        res.setHeader("Content-Length", buff);
    }
    if (!contentTypeFlag)
        res.setHeader("Content-Type", "text/html");
}

void CGI::read_cgi_response()
{
    char buff2[1025];
    fcntl(rPipe, F_SETFL, O_NONBLOCK);
    int nbytes = read(rPipe, buff2, 1024);
    if (nbytes < 0)
    {
        std::cout << "read fail!!" << std::endl;
        return ;
    }
    buff2[nbytes] = '\0';
    ResBody += buff2;
    while ((nbytes = read(rPipe, buff2, 1024)) > 0) {
        buff2[nbytes] = '\0';
        ResBody += buff2;
    }
}

void CGI::sendServerResponse(int fd, Config& config)
{
    cgiRes += config.getClients()[fd].getResponse().getStatusMssg();
    
    //check if there is already some http headers defined inside the script
    findHeadersInsideScript(config.getClients()[fd].getResponse());

    std::map<std::string, std::string>::iterator it = config.getClients()[fd].getResponse().getHeadersRes().begin();
    while (it != config.getClients()[fd].getResponse().getHeadersRes().end()) {
        std::string header = it->first + ": " + it->second;
        cgiRes += header + "\r\n";
        it++;
    }

    cgiRes += "\r\n";

    if (ResBody.length() > 0)
        cgiRes += ResBody;

    std::cout << "------------cgi Response------------------"<<std::endl;
    std::cout << cgiRes<< std::endl;

    send(fd, cgiRes.c_str(), cgiRes.length(), 0);
    
    std::cout << "cgi script sent!!"<<std::endl;
} 

void CGI::execute_cgi_script(Config& config, Response& res, int fd, Request req)
{
    (void)fd;

    //set status mssg
    defineResponseStatusMssg(res);

    //initialize env variables
    initializeVars(res, req);

    //set environment variables 
    setEnvVars(req, res);

    //find the absolute path of the script
    findExecutablePath();

    int fds[2];
    if (!config.getTimeoutResponseFlag()) {
        std::cout << "cgi script executing!!" << std::endl;
        int save_out = dup(1);
        if (pipe(fds) != 0)
            std::cerr << "pipe failed!!" << std::endl;
        pid_t c_fd = fork();
        if (c_fd != 0) {
            setStartTime(Config::timeNow());
            setCpid(c_fd);
            setRpipe(fds[0]);
        }
        if (!c_fd) {
            close(fds[0]);
            dup2(fds[1], 1);
            close(fds[1]);
            if (execve(executablePathArray, argv, envs) ==-1) {
                std::cerr << "execve failed!!" << std::endl;
                std::cerr << strerror(errno) << std::endl;
                exit(1);
            }
            dup2(save_out, 1);
            close(save_out);
        }
        close(fds[1]);
        close(save_out);



        //set child variables and check child status
    }
}
