/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/26 16:28:03 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../Config/Config.hpp"
//error pages
//if an error is not matched you should use a default
//you should match the error pages in root

//location (list or not)
//1- match request url with location url
//2- concatenate path request with location root
//3- if index.html in the dir = not list
//4- if not index.html is there check autoindex on or off
//5- if not index.html and not autoindex = 404 not found

//error pages
//you can only define custom error pages in 300 - 599
//1- the server always search for error pages from the root of the location
//2- should add 302 moved temporarily when redirecting to the defined error page
//3- if no leading slash means you should send in the location the location as it is with no slash and if the slash is there you should give the host in the location


int    Response::comparingReqWithLocation(std::string locationPath, std::string reqPath) {
    // std::cout << locationPath << std::endl;
    // std::cout << reqPath << std::endl;
    size_t i = locationPath.find(reqPath);
    std::string rest = locationPath.substr(i);
    return (0);
}

void    Response::showIndexFile(std::string indexFilePath)
{
    indexFile = new std::ifstream(indexFilePath.c_str());
    std::string buff;
      statusCode = 200;
    while (std::getline(*indexFile, buff))
        body += buff;
    indexFile->close();
}

std::string Response::urlEncode(std::string path)
{
    std::string res;
    for (std::string::iterator i = path.begin(); i != path.end(); i++)
    {
        if (!isalnum(*i) && *i != '-' && *i != '_' && *i != '.' && *i != '~' && *i != '/')
        {
            int n = static_cast<int>(*i);
            std::stringstream s;
            s << '%' << std::setw(2) << std::setfill('0') << std::uppercase 
                << std::hex << n << std::nouppercase;
            res += s.str();
        }
        else 
            res += *i;
    }
    return (res);
}

void Response::listDirectories(std::string reqPath)
{
    struct dirent *stDir;
    struct stat   st;
    std::string row;
    std::string dirAsbolute = currentDirAbsolutePath + reqPath;
    std::string direntName;
    std::string direntPath;
    std::string EncodedPath;

    statusCode = 200;
    Headers["Content-Type"] = "text/html";
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>p {text-align:left;font-size: small} h1 {text-align: left; font-size: large}</style>"
                "</head>"
                "<body>"
                "<h1>Webserv</h1>"
                "<hr></hr>";
    
    while ((stDir = readdir(dir)))
    {
        direntName = stDir->d_name;
        if (direntName != ".")
        {
            direntPath = reqPath + direntName;
            EncodedPath = urlEncode(reqPath + direntName);
            std::string absoluteDirentPath = currentDirAbsolutePath + direntPath;
            if (!stat(absoluteDirentPath.c_str(), &st) && st.st_mode & S_IFDIR)
            {
                EncodedPath += "/";
                direntName += "/";
            }
            row = "<a href=\"" + EncodedPath + "\">" + "<p>" + direntName;
            row += "</p></a>";
            lDirectoriesPage += row;
        }
    }
    lDirectoriesPage += "</body>";
    lDirectoriesPage += "</html>";
    
    body = lDirectoriesPage;
    closedir(dir);
}

void Response::matchReqPathWithLocation(Location& loc, std::string reqPath, Location **match)
{
    std::string uri;
    size_t index;

    uri = loc.getURI();
    index = uri.rfind("/");
    if ((index == 0 || index != uri.length() - 1) && uri.length() != 1)
    {
        uri = uri + "/";
        loc.setURI(uri);
    } 
    index = reqPath.rfind("/");
    if ((index == 0 || index != reqPath.length() - 1) && reqPath.length() != 1)
        reqPath = reqPath + "/";

    if (uri.length() > 0 && uri[0] == '/' && !strncmp(uri.c_str(), reqPath.c_str(), uri.length()))
    {  //dir should have a leading slash
        if (!*match || (*match && loc.getURI().length() > (*match)->getURI().length()))
            *match = &loc;
    }
}

void Response::listingOrIndex(Config& config, std::string reqPath)
{
  struct stat st;
  std::string locationPath;
  std::string indexFile;
  std::string uri;
  std::string root;
  std::string pathMatch;
  std::string serverRoot;

  serverRoot = config.getClients()[clientFd].getServer().getRoot();
  if (locationMatch)
  {
      uri = locationMatch->getURI();
      root = locationMatch->getRoot();
      if (root != "/")
          pathMatch = root + uri;
      else
          pathMatch = uri;
      
      indexFile = pathMatch + locationMatch->getIndex();
  }
  else 
      indexFile = serverRoot + reqPath + config.getClients()[clientFd].getServer().getIndex();

  if (locationMatch)
  {
      if (locationMatch->getAutoindex())
      {
          if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
              showIndexFile(indexFile);
          else
              listDirectories(reqPath);
      }
      else if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
          showIndexFile(indexFile);
  }
  else 
  {
      std::cout << "index file -> " << indexFile << std::endl;
      if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
          showIndexFile(indexFile);
      else if (config.getClients()[clientFd].getServer().getAutoindex())
          listDirectories(reqPath);
  }
}

void Response::checkAutoIndex(Config& config, Request& req)
{
    searchLocationsForMatch(config, req); 
    listingOrIndex(config, req.getPath());
}

void Response::searchLocationsForMatch(Config& config, Request& req)
{
    std::vector<Location>::iterator itLocations = config.getClients()[clientFd].getServer().getLocations().begin();
    while (itLocations != config.getClients()[clientFd].getServer().getLocations().end())
    {
        matchReqPathWithLocation(*itLocations, req.getPath(), &locationMatch);
        itLocations++;
    } 
}

int    Response::checkDefinedErrorPage(Config& config, std::string rootPath, std::map<int, std::string> error_page)
{
    std::map<int, std::string>::iterator it = error_page.begin();
    while (it != error_page.end())
    {
        if (it->first == statusCode)
        {
            if (!it->second.empty() && it->second.find("/") != 0)
            {
                statusCode = 302;
                locationHeader = it->second;
            }
            else
            {
              returnDefinedPage(config, rootPath, it->second);
              return (1);
            }
        }
        it++;
    }
    return (0);
}

void Response::checkErrorPages(Config& config, Request& req)
{
  searchLocationsForMatch(config, req);
  if (locationMatch)
  {
      if (checkDefinedErrorPage(config, config.getClients()[clientFd].getServer().getRoot(),
            locationMatch->getErrorPage()))
      { 
          return ;
      }
  }
  else
  {
      checkDefinedErrorPage(config, config.getClients()[clientFd].getServer().getRoot(),
        config.getClients()[clientFd].getServer().getErrorPage());
  }
}

void    Response::returnDefinedPage(Config& config, std::string rootPath, std::string errorPageFile)
{
    (void)config;
    std::string buffer;
    struct stat st;
    std::string relativeErrorPage = errorPageFile;

    if (rootPath != "/")
    {
        if (rootPath.rfind("/") == rootPath.length() - 1)
            rootPath.erase(0, 1);
        errorPageFile = rootPath + errorPageFile; 
    }
    int res = stat(errorPageFile.c_str(), &st);
    if (!res && (st.st_mode & S_IRUSR) && (st.st_mode & S_IFDIR))
    {
        clearResponse();
        statusCode = 301;
        locationHeader = relativeErrorPage;
        return ;
    }
    else if (!res && !(st.st_mode & S_IRUSR))
    {
        clearResponse();
        statusCode = 403;
        return ; 
    }
    else if (res == -1)
    {
        clearResponse();
        statusCode = 404;
        return ;
    }
    errorPage = new std::ifstream(errorPageFile.c_str());
    if (!errorPage->is_open())
    {
        std::cerr << "Error: Opening the Error file" << std::endl;
        clearResponse();
        statusCode = 500;
        return ;
    }
    while (std::getline(*errorPage, buffer))
        body += buffer;
    errorPage->close();
}

int Response::callbackRemove(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    (void)sb;
    (void)ftwbuf;
    (void)typeflag;

    int rv;
    rv = remove(fpath);
    if (rv)
        perror(fpath);

    return rv;
}

int Response::rmrf(char *path)
{
    return nftw(path, callbackRemove, 64, FTW_DEPTH | FTW_PHYS);
}
