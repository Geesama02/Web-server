/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        ++  +:+       ++        */
/*                                                +++#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:44 by maglagal          +    #+#             */
/*   Updated: 2025/02/23 11:52:12 by maglagal         #   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "../Config/Config.hpp"

CGI::CGI()
{
    cPid = 0;
    envsNbr = 0;
    outFileFd = 0;
    envs = NULL;
    executablePathArray = NULL;
    absoluteFilePath = NULL;
    childStatus = 0;
}

CGI::~CGI() 
{
   clearCGI(); 
}

//getters
std::string  CGI::getBody() { return ResBody; }
int          CGI::getChildStatus() { return childStatus; }
pid_t        CGI::getCpid() { return cPid; }
int          CGI::getRpipe() { return rPipe; }
long long    CGI::getStartTime() { return startTime; }
std::string& CGI::getPathInfo() {return pathInfo;}
std::string& CGI::getscriptFilePath() {return scriptFilePath; }
std::string& CGI::getExtensionFile() {return extensionFile;}
int          CGI::getOutFileFd() {return outFileFd;}
std::string& CGI::getOutFileName() {return outFileName;}

//setters
void        CGI::setBody(std::string newBody) { ResBody = newBody; }
void        CGI::setChildStatus(int newChildStatus) { childStatus = newChildStatus; }
void        CGI::setCpid(pid_t nPid) { cPid = nPid; }
void        CGI::setRpipe(int nRpipe) { rPipe = nRpipe; }
void        CGI::setStartTime(long long nTime) {startTime = nTime;}
void        CGI::setPathInfo(std::string nValue) {pathInfo = nValue;}
void        CGI::setscriptFilePath(std::string nValue) { scriptFilePath = nValue; }
void        CGI::setExtensionFile(std::string nValue) {extensionFile = nValue;}
void        CGI::setOutFileFd(int nFd) {outFileFd = nFd;}
void        CGI::setOutFileName(std::string& nValue) {outFileName = nValue;}


//define cgi execution paths from config file
void CGI::defineExecutionPaths(int fd, Config& config)
{
    std::vector<std::string>::iterator extensionIt = config.getClients()[fd].getServer().getCgiExt().begin();
    std::vector<std::string>::iterator pathIt = config.getClients()[fd].getServer().getCgiPath().begin();
    while(extensionIt != config.getClients()[fd].getServer().getCgiExt().end()
        && pathIt != config.getClients()[fd].getServer().getCgiPath().end())
    {
        executablePaths[*extensionIt] = *pathIt;
        extensionIt++;
        pathIt++;
    }
}

void CGI::clearCGI()
{
    if (envs) 
    {
        while(envsNbr--) 
        {
            if (envs[envsNbr])
                delete[] envs[envsNbr];
        }
        delete[] envs;
        envs = NULL;
    }
    if (absoluteFilePath)
        delete[] absoluteFilePath;
    if (executablePathArray)
        delete[] executablePathArray;
    executablePathArray = NULL;
    absoluteFilePath = NULL;
    if (outFileFd != 0)
    {   
        close(outFileFd);
        if (outFilePath.length() > 0)
            remove(outFilePath.c_str());
    }
    cPid = 0;
    rPipe = 0;
    startTime = 0;
    outFileFd = 0;
    childStatus = 0;
    envsNbr = 0;
    outFileName.clear();
    outFilePath.clear();
    headersInScript.clear();
    ResBody.clear();
    cgiRes.clear();
    extensionFile.clear();
    scriptFilePath.clear();
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

void CGI::checkHeaderName(std::string& headerName)
{
    for(size_t i = 0; i < headerName.length(); i++)
    {
        if (isalpha(headerName[i]) && islower(headerName[i]))
            headerName[i] = toupper(headerName[i]);
        else if(headerName[i] == '-')
            headerName[i] = '_';
    }
}

int CGI::addMetaVariables(Config& config, Request& req, Response& res)
{
    int i = 0;
    for (std::map<std::string, std::string>::iterator headersIt = req.getHeaders().begin(); headersIt != req.getHeaders().end(); headersIt++)
    {
        std::string headerName = headersIt->first;
        checkHeaderName(headerName);
        std::string env = "HTTP_" + headerName + "=" + headersIt->second;
        envs[i] = new(std::nothrow) char[env.length() + 1];
        std::strcpy(envs[i], env.c_str());
        i++;
        envsNbr++;
    }

    std::map<std::string, std::string>::iterator it = storeEnvs.begin();
    for (int j = 0; j < 14; j++)
    {
        std::string env = it->first + "=" + it->second;
        envs[i] = new(std::nothrow) char[env.length() + 1];
        if (!envs[i])
        {
            while(i--)
            {
                delete[] envs[i];
                envs[i] = NULL;
            }
            delete[] envs;
            envs = NULL;
            return (failureHandler(config, res.getClientFd()));
        }
        std::strcpy(envs[i], env.c_str());
        it++;
        i++;
    }
    envs[i] = NULL;
    envsNbr += 15;
    return (0);
}

void CGI::searchForScriptName(Config& config, int fd)
{
    std::string reqPath = config.getClients()[fd].getRequest().getPath();
    char portChar[150];
    int  port;
    if (pathInfo.length())
    {
        size_t index = reqPath.find(pathInfo);
        scriptFileName = reqPath.substr(0, index);
        if (config.getClients()[fd].getResponse().getQueryString().length() > 0)
            scriptFileName += "?"+ config.getClients()[fd].getResponse().getQueryString();
    }
    else
        scriptFileName = reqPath;
    port = config.getClients()[fd].getServer().getPort(); 
    sprintf(portChar, "%d", port);
    std::string host = config.getClients()[fd].getServer().getHost() + ":" + portChar;
    scriptFileName = "http://" + host + scriptFileName;
}

int CGI::setEnvVars(Config& config, Request& req, Response& res, int fd)
{
    char contentLengthStr[150];
    if (req.getBody().length() > 0)
    sprintf(contentLengthStr, "%ld", req.getBody().length());
    
    char portChar[150];
    sprintf(portChar, "%d", config.getClients()[res.getClientFd()].getServer().getPort());
    searchForScriptName(config, fd);
    storeEnvs["REQUEST_METHOD"] = req.getMethod().c_str();
    storeEnvs["QUERY_STRING"] = (res.getQueryString()).c_str();
    storeEnvs["REMOTE_HOST"] = config.getClients()[res.getClientFd()].getServer().getHost();
    storeEnvs["REMOTE_ADDR"] = config.getClients()[res.getClientFd()].getClientIP();
    if (req.getPath().empty())
        storeEnvs["SCRIPT_NAME"] = "/";
    else
        storeEnvs["SCRIPT_NAME"] = scriptFileName;
    storeEnvs["SERVER_NAME"] = "Webserv";
    storeEnvs["SERVER_PROTOCOL"] = "HTTP 1.1";
    storeEnvs["SERVER_PORT"] = portChar;
    storeEnvs["SERVER_SOFTWARE"] = "Webserv/1.1";
    storeEnvs["GATEWAY_INTERAFCE"] = "CGI/1.1";
    if (req.getMethod() == "GET")
        storeEnvs["CONTENT_LENGTH"] = "0"; //forbidden!!
    else if (req.getMethod() == "POST")
        storeEnvs["CONTENT_LENGTH"] = req.getHeaders()["content-length"].c_str(); //forbidden!! 
    storeEnvs["CONTENT_TYPE"] = req.getHeaders()["content-type"].c_str();
    if (pathInfo.length() > 0) {
        storeEnvs["PATH_INFO"] = pathInfo.c_str();
        storeEnvs["PATH_TRANSLATED"] = ((scriptFilePath.substr(0, scriptFilePath.rfind("/"))) + pathInfo).c_str();
    }
    else {
        storeEnvs["PATH_INFO"] = "";
        storeEnvs["PATH_TRANSLATED"] = "";
    }
    
    envs = new(std::nothrow) char*[req.getHeaders().size() + 15];
    if (!envs)
        return (failureHandler(config, fd));

    addMetaVariables(config, req, res);
    return (0);
}

int CGI::defineArgv(Config& config, int fd)
{
    absoluteFilePath = new(std::nothrow) char[scriptFilePath.length() + 1];
    if (!absoluteFilePath)
        return (failureHandler(config, fd));
    std::strcpy(absoluteFilePath, scriptFilePath.c_str());
    argv[0] = executablePathArray;
    argv[1] = absoluteFilePath;
    argv[2] = NULL;
    return (0);
}

int CGI::findExecutablePath(Config& config, int fd) 
{
    std::map<std::string, std::string>::iterator it = executablePaths.begin();
    while (it != executablePaths.end() && it->first != extensionFile)
      it++;
    if (it == executablePaths.end())
    {
        std::cerr << "Error : Extension file not found" << std::endl;
        return (failureHandler(config, fd));
    }
    std::string executablePath = it->second;
    executablePathArray = new(std::nothrow) char[executablePath.length() + 1];
    if (!executablePathArray)
        return failureHandler(config, fd);
    std::strcpy(executablePathArray, executablePath.c_str());
    return (0);
}


void CGI::findHeadersInsideScript(Response& res) {
    int         contentLengthFlag = 0;
    int         contentTypeFlag = 0;
    std::string headerInScript;
    std::string headerName;
    std::string headerValue;

    size_t i = ResBody.find("\r\n\r\n");
    if (i == std::string::npos)
        i = ResBody.find("\n\n");
    if (i != std::string::npos)
    {
        headerInScript = ResBody.substr(0, i);
        ResBody.erase(0, i + 1);
    }
    if (headerInScript.length() > 0)
    {
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
     while(i != std::string::npos && ResBody[0] == '\n')
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

    if (fcntl(outFileFd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error: Fcntl Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    // Reset the file pointer to the beginning of the file
    if (lseek(outFileFd, 0, SEEK_SET) == -1) {
        std::cerr << "Error: lseek Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    int nbytes = read(outFileFd, buff, 1024);
    if (nbytes < 0)
    {
        std::cerr << "Error: Read Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    buff[nbytes] = '\0';
    ResBody += buff;
    while ((nbytes = read(outFileFd, buff, 1024)) > 0)
    {
        if (nbytes < 0)
        {
            std::cerr << "Error: Read Failed" << std::endl;
            return (failureHandler(config, fd));
        }
        buff[nbytes] = '\0';
        ResBody += buff;
    }
    return (0);
}

void CGI::sendServerResponse(int fd, Config& config)
{
    cgiRes += config.getClients()[fd].getResponse().getStatusMssg();
    config.getClients()[fd].getResponse().setHeader("Date", Response::getDate());
    
    //check if there is already some http headers defined inside the script
    findHeadersInsideScript(config.getClients()[fd].getResponse());
    std::map<std::string, std::string>::iterator it = config.getClients()[fd].getResponse().getHeadersRes().begin();
    while (it != config.getClients()[fd].getResponse().getHeadersRes().end())
    {
        std::string header = it->first + ": " + it->second;
        cgiRes += header + "\r\n";
        it++;
    }
    cgiRes += "\r\n";
    if (ResBody.length() > 0)
        cgiRes += ResBody;

    send(fd, cgiRes.c_str(), cgiRes.length(), 0);    
} 

void CGI::generateFileName()
{
    struct timeval tv;
    std::ostringstream time;
    gettimeofday(&tv, NULL);
    time << (tv.tv_sec * 1000000) + tv.tv_usec;
    outFileName.insert(outFileName.size(), "_" + time.str());
}

int CGI::creatingOutFile()
{
    generateFileName();
    char *user = std::getenv("USER");
    if (user) {
        std::string userStr = user;
        outFilePath = "/home/" + userStr + "/goinfre/" + '.' + outFileName;
    }
    else
        outFilePath = "/tmp/" + '.' + outFileName;
    FILE* fileOut = fopen(outFilePath.c_str(), "w+");
    if (!fileOut)
        return (-1);
    outFileFd = fileno(fileOut);
    return (0);
}

int CGI::execute_cgi_script(Config& config, Response& res, int fd, Request req)
{
    //set executable paths
    defineExecutionPaths(fd, config);

    //set status mssg
    defineResponseStatusMssg(res);
    
    //set environment variables 
    if (setEnvVars(config, req, res, fd) == -1)
        return (-1);

    //find the absolute path of the script
    if (findExecutablePath(config, fd) == -1)
        return (-1);

    //define argv to pass it to execve
    if (defineArgv(config, fd) == -1)
        return (-1);

    if (creatingOutFile() == -1)
    {     
        std::cerr << "error creating out file!!" << std::endl;
        return failureHandler(config, fd);
    }
    std::cout << "file name -> " << outFileName << std::endl;
    // int fds[2];
    int save_out = dup(1);
    if (save_out == -1)
    {
        std::cerr << "Error: Dup Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    int save_in = dup(0);
    if (save_in == -1)
    {
        std::cerr << "Error: Dup Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    // if (pipe(fds) != 0)
    // { 
    //     std::cerr << "Error: Pipe Failed" << std::endl;
    //     return (failureHandler(config, fd));
    // }
    pid_t c_pid = fork();
    if (c_pid == -1)
    { //pipes already opened we should close
        std::cerr << "Error: Fork Failed" << std::endl;
        return (failureHandler(config, fd));
    }
    if (c_pid != 0)
    {
        std::cout << "pid -> " << c_pid << std::endl;
        startTime = Config::timeNow();
        cPid = c_pid;
    }
    if (!c_pid)
    {
        if (req.getMethod() == "POST")
        {
            int bodyFd = open(req.getFileName().c_str(), S_IRUSR);
            if (bodyFd == -1)
                exit(EXIT_FAILURE);
            if (dup2(bodyFd, 0) == -1)
                exit(EXIT_FAILURE);
            close(bodyFd);
            remove(req.getFileName().c_str());
        }
        if (dup2(outFileFd, 1) == -1)
            exit(EXIT_FAILURE);
        close(outFileFd);
        if (execve(executablePathArray, argv, envs) == -1)
            std::cerr << "Error Execve : " << strerror(errno) << std::endl;
        dup2(save_out, 1);
        dup2(save_in, 0);
        close(save_out);
        close(save_in);
        exit(EXIT_FAILURE);
    }
    close(save_out);
    close(save_in);
    return (0);
}
