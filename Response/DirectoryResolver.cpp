/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/03/11 17:53:19 by maglagal         ###   ########.fr       */
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


int    Response::comparingReqWithLocation(std::string locationPath, std::string reqPath)
{
    size_t i = locationPath.find(reqPath);
    std::string rest = locationPath.substr(i);
    return (0);
}

void    Response::showIndexFile(Config& config, std::string indexFilePath, Request& req)
{
    (void)config;
    std::string extensionFile = indexFilePath;
    // std::string cgiDir = config.getClients()[clientFd].getServer().getCgiDir();
    // if (*cgiDir.rbegin() != '/')
    //     cgiDir += '/'; // need to fix
    // if (!strncmp(req.getPath().c_str(), cgiDir.c_str(), cgiDir.length()))
    //     return (handleCgiScript(config, reqResolved));
    filePath = indexFilePath;
    checkForFileExtension(extensionFile);
    if (req.getHeaders().find("range") != req.getHeaders().end())
        statusCode = 206;
    else
        statusCode = 200;
    file = new(std::nothrow) std::ifstream(indexFilePath.c_str());
    if (!file)
    {
        clearResponse();
        statusCode = 500;
        return ;
    }
    bytesToSend = req.strToDecimal(Headers["Content-Length"]);
    Headers["Accept-Ranges"] = "bytes";
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

void Response::listDirectories(Request& req, std::string dirAsbolute)
{
    struct dirent *stDir;
    struct stat   st;
    std::string row;
    std::string direntName;
    std::string direntPath;
    std::string EncodedPath;

    statusCode = 200;
    Headers["Content-Type"] = "text/html";
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>p {text-align:left;font-size:medium;padding-left:12px;;padding-right:12px;} h1 {text-align: left;padding-left:12px;padding-right:12px;}</style>"
                "</head>"
                "<body>"
                "<h1>Index Of ";
    
    lDirectoriesPage += dirAsbolute + "</h1>" + "<hr></hr>";
    while ((stDir = readdir(dir)))
    {
        direntName = stDir->d_name;
        if (direntName != "." && (*direntName.begin() != '.' || direntName == ".."))
        {
            direntPath = req.getPath() + direntName;
            EncodedPath = urlEncode(direntPath);
            std::string absoluteDirentPath = dirAsbolute + direntPath;
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
    closedir(dir);
    body = lDirectoriesPage;
}

void Response::matchReqPathWithLocation(Location& loc, std::string reqPath, Location **match)
{
    std::string uri;

    uri = loc.getURI();
    if (uri.length() > 0 && uri[0] == '/' && !strncmp(uri.c_str(), reqPath.c_str(), uri.length()))
    {  //dir should have a leading slash
        if (!*match || (*match && loc.getURI().length() > (*match)->getURI().length()))
            *match = &loc;
    }
}

void Response::verifyInfiniteRedirections(std::string toCheck)
{
    for (std::vector<std::string>::iterator it = savedRedirects.begin(); it != savedRedirects.end(); it++)
    {
        if (*it == toCheck) {
            statusCode = 500;
            return ;
        }
    }
}

void    Response::updateIndexFilePath(Config& config, std::string& indexFile, std::string& locationIndex, std::string& pathMatch)
{
    (void)pathMatch;
    (void)locationIndex;
    (void)indexFile;
    (void)config;
    std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
    std::string uri = locationMatch->getURI();
    std::string root = locationMatch->getRoot();
    
}


void Response::IndexFileLogic(Config& config, Request& req)
{
  struct stat st;
  std::string locationPath;
  std::string indexFile;
  std::string uri;
  std::string root;
  std::string pathMatch;
  std::string serverRoot;
  std::string locationIndex;
  Location* locationIndexMatch = NULL;

  serverRoot = config.getClients()[clientFd].getServer().getRoot();
  if (locationMatch)
  {
        locationIndex = locationMatch->getIndex();
        uri = locationMatch->getURI();
        root = locationMatch->getRoot();
        locationIndexMatch = Request::getMatchedLocation(locationIndex, config.getClients()[clientFd].getServer());

        if (locationIndexMatch && *locationIndex.begin() == '/' && *locationIndex.rbegin() == '/')
        {
            locationMatch = locationIndexMatch;
            req.setPath(locationIndex);
            verifyInfiniteRedirections(locationIndex);
            if (statusCode == 500)
            {
                clearResponse();
                statusCode = 500;
                return ;
            }
            savedRedirects.push_back(locationIndex);
            IndexFileLogic(config, req);
            return ;
        }
        else if (*locationIndex.begin() == '/' && *locationIndex.rbegin() != '/')
        {
            // updateIndexFilePath(config, indexFile, locationIndex, pathMatch);
            if (root != "/")
                pathMatch = root + uri;
            else
                pathMatch = uri;
            if (*locationIndex.begin() == '/')
                indexFile = serverRoot + locationIndex;
            else
            {
                locationIndex = "/" + locationIndex;
                indexFile = pathMatch + locationIndex;
            }
            if (!stat(indexFile.c_str(), &st) && (st.st_mode & S_IFREG))
                reqResolved = root + req.getPath();
            else if (errno == ENOENT)
            {
                clearResponse();
                statusCode = 404;
                return ;
            }
            else
            {
                statusCode = 301;
                redirectFlag = 1;
                locationHeader = locationIndex + '/';
                return ;
            }
        }
        else
        {
            if (!locationIndexMatch)
                req.setPath(locationIndex);
            locationMatch = locationIndexMatch;
            if (root != "/")
                pathMatch = root + uri;
            else
                pathMatch = uri;
            if (*pathMatch.rbegin() != '/' && *req.getPath().begin() != '/')
                pathMatch += '/';
            reqResolved = pathMatch + req.getPath();
            if (!stat(reqResolved.c_str(), &st))
            {
                if (!(st.st_mode & S_IFDIR))
                    reqResolved = pathMatch;
            }
            else
            {
                if (!stat(pathMatch.c_str(), &st) && (st.st_mode & S_IFDIR))
                    reqResolved = pathMatch;
                else
                {
                    clearResponse();
                    statusCode = 404;
                    return ;
                }
            }
            if (*locationIndex.begin() == '/')
                indexFile = serverRoot + locationIndex;
            else
            {
                locationIndex = "/" + locationIndex;
                indexFile = pathMatch + locationIndex;
            }
        }
  }
  else 
        indexFile = serverRoot + req.getPath() + config.getClients()[clientFd].getServer().getIndex();

  listOrIndex(config, req, indexFile);

}

void Response::listOrIndex(Config &config, Request& req, std::string& indexFile)
{
  struct stat st;
  if (locationMatch)
  {
      if (locationMatch->getAutoindex())
      {
          if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
          {
            lastModified = getDate(&st.st_mtime);
            sprintf(contentLengthHeader, "%ld", st.st_size);
            setHeader("Content-Length", contentLengthHeader);
            showIndexFile(config, indexFile, req);
          }
          else if (!stat(reqResolved.c_str(), &st) && st.st_mode & S_IFDIR)
            listDirectories(req, reqResolved);
      }
      else if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
      {
          lastModified = getDate(&st.st_mtime);
          sprintf(contentLengthHeader, "%ld", st.st_size);
          setHeader("Content-Length", contentLengthHeader);
          showIndexFile(config, indexFile, req);
      }
  }
  else 
  {
      if (!stat(indexFile.c_str(), &st) && st.st_mode & S_IFREG)
      {
          lastModified = getDate(&st.st_mtime);
          sprintf(contentLengthHeader, "%ld", st.st_size);
          setHeader("Content-Length", contentLengthHeader);
          showIndexFile(config, indexFile, req);
      }
      else if (!stat(reqResolved.c_str(), &st) && st.st_mode & S_IFDIR
            && config.getClients()[clientFd].getServer().getAutoindex())
        listDirectories(req, reqResolved);
  }
}

void Response::checkAutoIndex(Config& config, Request& req)
{
    searchLocationsForMatch(config, req);
    IndexFileLogic(config, req);
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
              returnDefinedPage(config, it->first, rootPath, it->second);
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
        checkDefinedErrorPage(config, config.getClients()[clientFd].getServer().getRoot(),
        locationMatch->getErrorPage());
    }
    else
    {
        checkDefinedErrorPage(config, config.getClients()[clientFd].getServer().getRoot(),
          config.getClients()[clientFd].getServer().getErrorPage());
    }
}

void    Response::returnDefinedPage(Config& config, int errorStatus, std::string rootPath, std::string errorPageFile)
{
    std::string buffer;
    struct stat st;
    std::string relativeErrorPage = errorPageFile;

    locationMatch = Request::getMatchedLocation(relativeErrorPage, config.getClients()[clientFd].getServer());
    if (*errorPageFile.begin() == '/' && locationMatch)
        rootPath = locationMatch->getRoot();
    if (rootPath != "/")
    {
        if (rootPath.rfind("/") == rootPath.length() - 1)
            rootPath.erase(0, 1);
        errorPageFile = rootPath + errorPageFile;
    }
    int res = stat(errorPageFile.c_str(), &st);
    if (!res && locationMatch)
    {
        body.clear();
        if (locationMatch->getRedirect().size() > 0)
        {
            if ((locationMatch->getRedirect().begin()->first >= 301
                && locationMatch->getRedirect().begin()->first <= 303)
                || locationMatch->getRedirect().begin()->first == 307
                || locationMatch->getRedirect().begin()->first == 308)
                statusCode = locationMatch->getRedirect().begin()->first;
            else
            {    
                statusCode = errorStatus;
                body = locationMatch->getRedirect().begin()->second;
            }
            checkForFileExtension(relativeErrorPage);
            redirectFlag = 1;
            return ;
        }
        else
        {
            if (st.st_mode & S_IFREG)
                statusCode = errorStatus;
        //     else {
        //         locationHeader = locationMatch->getURI();
        //         std::cout << "dir !!\n";
        //         statusCode = 301;
        //     }
        }
        // if (statusCode == 301)
        //     return ;
    }
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
    errorPage = new(std::nothrow) std::ifstream(errorPageFile.c_str());
    if (!errorPage || !errorPage->is_open())
    {
        std::cerr << "Error: Opening the Error Page" << std::endl;
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

void Response::returnResponse(Config& config)
{
    searchLocationsForMatch(config, config.getClients()[clientFd].getRequest());
    if (locationMatch)
    {
        std::map<int, std::string> redirect = locationMatch->getRedirect();
        std::map<int, std::string>::iterator redirectIt = redirect.begin();
        if (redirectIt != redirect.end())
        {
            redirectFlag = 1;
            statusCode = redirectIt->first;
            if ((statusCode >= 301 && statusCode <= 303)
                || statusCode == 307 || statusCode == 308)
                locationHeader = redirectIt->second;
            else
                body = redirectIt->second;
            Headers["Content-Type"] = "application/octet-stream";
        }
    }
    else
    {
       std::map<int, std::string> redirect = config.getClients()[clientFd].getServer().getRedirect();
       std::map<int, std::string>::iterator redirectIt = redirect.begin();
       if (redirectIt != redirect.end())
        {
            redirectFlag = 1;
            statusCode = redirectIt->first;
            if ((statusCode >= 301 && statusCode <= 303)
                || statusCode == 307 || statusCode == 308)
                locationHeader = redirectIt->second;
            else
                body = redirectIt->second;
            Headers["Content-Type"] = "application/octet-stream";
        }
    }
}
