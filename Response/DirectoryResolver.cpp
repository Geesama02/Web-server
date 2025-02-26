/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/25 16:59:51 by oait-laa         ###   ########.fr       */
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
    std::ifstream indexFile(indexFilePath.c_str());
    std::string buff;
      statusCode = 200;
    while (std::getline(indexFile, buff))
        body += buff;
}

std::string Response::urlEncode(std::string path)
{
    std::string res;
    for (std::string::iterator i = path.begin(); i != path.end(); i++) {
        if (!isalnum(*i) && *i != '-' && *i != '_' && *i != '.' && *i != '~' && *i != '/') {
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
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>p {text-align:left;font-size: small} h1 {text-align: left; font-size: large}</style>"
                "</head>"
                "<body>"
                "<h1>Webserv</h1>"
                "<hr></hr>";
    
    while ((stDir = readdir(dir))) {
        direntName = stDir->d_name;
        if (direntName != ".") {
            direntPath = reqPath + direntName;
            EncodedPath = urlEncode(reqPath + direntName);
            // std::cout << "direntPath -> " << direntPath << std::endl;
            std::string absoluteDirentPath = currentDirAbsolutePath + direntPath;
            if (!stat(absoluteDirentPath.c_str(), &st) && st.st_mode & S_IFDIR) {
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
}


void Response::matchReqPathWithLocation(Location& loc, std::string reqPath, Location **match)
{
    std::string pathMatch;
    std::string root;
    std::string uri;

    uri = loc.getURI();
    if (uri.rfind("/") == 0 && uri.length() != 1)
    {
        uri = uri + "/";
        loc.setURI(uri);
    }
    root = loc.getRoot();
    if (root != "/")
        pathMatch = root + uri;
    else
        pathMatch = uri;
    if (uri.length() > 0 && uri[0] == '/' && !strncmp(uri.c_str(), reqPath.c_str(), uri.length()))
    {  //dir should have a leading slash
        
        if (!*match || (*match && loc.getURI().length() > (*match)->getURI().length()))
            *match = &loc;
    }
}

void Response::listingOrIndex(Config& config, std::string reqPath)
{
  struct stat st;
  struct stat reqPathCheck;
  std::string locationPath;
  std::string indexFile;

  if (locationMatch)
  {
      locationPath = locationMatch->getURI();
      indexFile = reqPath + locationMatch->getIndex();
  }
  else
  {
      locationPath = "/";
      indexFile = config.getClients()[clientFd].getServer().getIndex();
  }
  std::string aIndexFile = currentDirAbsolutePath + indexFile;
  std::string reqPathAsbsolute = currentDirAbsolutePath + reqPath;

  if (locationMatch)
  {
          if (!statusCode && !stat(reqPathAsbsolute.c_str(), &reqPathCheck)
                && (reqPathCheck.st_mode & S_IFDIR) && locationMatch->getAutoindex())
          {
              if (!stat(aIndexFile.c_str(), &st) && st.st_mode & S_IFREG)
                  showIndexFile(aIndexFile);
              else
                  listDirectories(reqPath);
          }
          else if (!statusCode && !stat(aIndexFile.c_str(), &st) && st.st_mode & S_IFREG)
              showIndexFile(aIndexFile);
          else if (checkDefinedErrorPage(config.getClients()[clientFd].getServer().getRoot(),
              locationMatch->getErrorPage())) // should Nothing enter just when not found
          {
              std::cerr << "error page location!!!" << std::endl;
              return ;
          }
  }
  else {
      checkDefinedErrorPage(config.getClients()[clientFd].getServer().getRoot(),
        config.getClients()[clientFd].getServer().getErrorPage());
  }
  
}

void Response::checkAutoIndex(Config& config, Request req) {
    Location*   matchLocation = NULL;

    std::vector<Location>::iterator itLocations = config.getClients()[clientFd].getServer().getLocations().begin();
    while (itLocations != config.getClients()[clientFd].getServer().getLocations().end())
    {    
        matchReqPathWithLocation(*itLocations, req.getPath(), &matchLocation);
        itLocations++;
    }
    locationMatch = matchLocation;
    listingOrIndex(config, req.getPath());
}

int    Response::checkDefinedErrorPage(std::string rootPath, std::map<int, std::string> error_page) {
    std::map<int, std::string>::iterator it = error_page.begin();
    while (it != error_page.end())
    {
        if (it->first == statusCode)
        {
            if (!it->second.empty() && it->second.rfind("/") != 0)
            {
                statusCode = 302;
                locationHeader = it->second;
            }
            else
            {
              returnDefinedPage(rootPath, it->second);
              return (1);
            }
        }
        it++;
    }
    return (0);
}

void    Response::returnDefinedPage(std::string rootPath, std::string errorPageFile) {
    std::string buffer;
    if (rootPath != "/")
        errorPageFile = rootPath + errorPageFile;
    std::cout << "Error page " << errorPageFile<< std::endl;
    std::ifstream definedPage(errorPageFile.c_str());
    if (!definedPage.is_open())
    {
        clearResponse();
        statusCode = 404;
        return ;
    }
    while (std::getline(definedPage, buffer))
        body += buffer;
}
