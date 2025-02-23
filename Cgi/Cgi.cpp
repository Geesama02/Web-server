/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:44 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/22 18:28:10 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "../Config/Config.hpp"

CGI::CGI() {
    for(int i = 0; i < 7; i++)
        envs[i] = NULL;
    executablePathArray = NULL;
    absoluteFilePath = NULL;
    childStatus = 0;
    timeout = 10000;
}

CGI::~CGI() {
    for(int i = 0; i < 7; i++) {
        if (envs[i]) {
            delete[] envs[i];
            envs[i] = NULL;
        }
    }
    if (absoluteFilePath)
        delete[] absoluteFilePath;
    absoluteFilePath = NULL;
    if (executablePathArray)
        delete[] executablePathArray;
    executablePathArray = NULL;
}

//getters
std::string CGI::getBody() { return ResBody; }
int         CGI::getChildStatus() { return childStatus; }
pid_t       CGI::getCpid() { return cPid; }
int         CGI::getRpipe() { return rPipe; }
pid_t       CGI::getTimeout() { return timeout; }
long long   CGI::getStartTime() { return startTime; }

//setters
void        CGI::setBody(std::string newBody) { ResBody = newBody; }
void        CGI::setChildStatus(int newChildStatus) { childStatus = newChildStatus; }
void        CGI::setCpid(pid_t nPid) { cPid = nPid; }
void        CGI::setRpipe(int nRpipe) { rPipe = nRpipe; }
void        CGI::setTimeout(int nTimeout) { timeout = nTimeout; }
void        CGI::setStartTime(long long nTime) {startTime = nTime;}


//define cgi execution paths from config file
void CGI::defineExecutionPaths(int fd, Config& config)
{
    std::vector<Location>::iterator it = config.getClients()[fd].getServer().getLocations().begin();
    while(it != config.getClients()[fd].getServer().getLocations().end())
    {
        if ((it->getURI() == "/cgi-bin"))
        {
            std::vector<std::string>::iterator extensionIt = it->getCgiExt().begin();
            std::vector<std::string>::iterator pathIt = it->getCgiPath().begin();
            while(extensionIt != it->getCgiExt().end()
                && pathIt != it->getCgiPath().end())
            {
                executablePaths[*extensionIt] = *pathIt;
                extensionIt++;
                pathIt++;
            }
        }
        it++;
    }
}

void CGI::clearCGI() {
    for(int i = 0; i < 7; i++) {
        if (envs[i])
            delete[] envs[i];
    }
    if (absoluteFilePath)
        delete[] absoluteFilePath;
    if (executablePathArray)
        delete[] executablePathArray;
    for(int i = 0; i < 7; i++)
        envs[i] = NULL;
    executablePathArray = NULL;
    absoluteFilePath = NULL;
    if (rPipe != 0)
        close(rPipe);
    cPid = 0;
    rPipe = 0;
    startTime = 0;
    childStatus = 0;
    headersInScript.clear();
    ResBody.clear();
    cgiRes.clear();
    extensionFile.clear();
    scriptFileName.clear();
    scriptRelativePath.clear();
}

int CGI::failureHandler(Config& config, int fd)
{
    clearCGI();
    config.getClients()[fd].getResponse().clearResponse();
    config.getClients()[fd].getResponse().setStatusCode(500);
    return (-1);
}

void CGI::defineResponseStatusMssg(Response& res)
{  
    if (res.getStatusCode() == 200)
        res.setStatusMssg("HTTP/1.1 200 OK\n");
    else if (res.getStatusCode() == 404)
        res.setStatusMssg("HTTP/1.1 404 Not Found\n");
}

void CGI::convertHeaderToCamelCase(std::string& value) {
    if (value[0] >= 97 && value[0] <= 122)
    {
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

void CGI::defineArgv()
{
    char buff[120];
    getcwd(buff, 120);
    std::string current_dir = buff;
    std::string absolutePath = current_dir + scriptRelativePath;
    absoluteFilePath = new char[absolutePath.length() + 1];
    std::strcpy(absoluteFilePath, absolutePath.c_str());
    argv[0] = executablePathArray;
    argv[1] = absoluteFilePath;
    argv[2] = NULL;
}

int CGI::findExecutablePath(Config& config, int fd) 
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
    while (it != executablePaths.end() && it->first != extensionFile)
        it++;
    if (it == executablePaths.end())
    {
        std::cerr << "Error : Extension file not found" << std::endl;
        return (failureHandler(config, fd));
    }
    std::string executablePath = it->second;
    executablePathArray = new char[executablePath.length() + 1];
    std::strcpy(executablePathArray, executablePath.c_str());
    return (0);
}


void CGI::findHeadersInsideScript(Response& res) {
    int         contentLengthFlag = 0;
    int         contentTypeFlag = 0;
    std::string headerInScript;
    std::string headerName;
    std::string headerValue;

    size_t i = ResBody.find("\n\n");
    if (i != std::string::npos) {
        headerInScript = ResBody.substr(0, i);
        ResBody.erase(0, i + 1);
    }
    if (headerInScript.length() > 0)
    {
        std::cout << "headers in script  => " << headerInScript << std::endl; 
        if (headerInScript.find(":") != std::string::npos)
        {
            headersInScript = Request::split(headerInScript, 0, '\n');
            std::vector<std::string>::iterator it = headersInScript.begin();
            while (it != headersInScript.end())
            {
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

int CGI::read_cgi_response(Config& config, int fd)
{
    char buff[1025];
    if (fcntl(rPipe, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error: Fcntl Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    int nbytes = read(rPipe, buff, 1024);
    if (nbytes < 0)
    {
        std::cout << "Error: Read Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    buff[nbytes] = '\0';
    ResBody += buff;
    while ((nbytes = read(rPipe, buff, 1024)) > 0) {
        buff[nbytes] = '\0';
        ResBody += buff;
    }
    return (0);
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
    send(fd, cgiRes.c_str(), cgiRes.length(), 0);    
} 

int CGI::execute_cgi_script(Config& config, Response& res, int fd, Request req)
{
    //set executable paths
    defineExecutionPaths(fd, config);

    //set status mssg
    defineResponseStatusMssg(res);

    //initialize env variables
    initializeVars(res, req);

    //set environment variables 
    setEnvVars(req, res);

    //find the absolute path of the script
    if (findExecutablePath(config, fd) == -1)
        return (-1);

    //define argv to pass it to execve
    defineArgv();

    int fds[2];
    int save_out = dup(1);
    if (save_out == -1) {
        std::cerr << "Error: Pipe Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    if (pipe(fds) != 0)
    { 
        std::cerr << "Error: Pipe Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    pid_t c_pid = fork();
    if (c_pid == -1)
    {
        std::cerr << "Error: Fork Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    if (c_pid != 0)
    {
        startTime = Config::timeNow();
        cPid = c_pid;
        rPipe = fds[0];
    }
    if (!c_pid)
    {
        close(fds[0]);
        if (dup2(fds[1], 1) == -1)
            exit(EXIT_FAILURE);
        close(fds[1]);
        if (execve(executablePathArray, argv, envs) == -1)
        {
            std::cerr << "Error Execve : " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        dup2(save_out, 1);
        close(save_out);
    }
    close(fds[1]);
    close(save_out);
    return (0);
}
