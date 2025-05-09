/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 14:31:56 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/13 21:18:50 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

int Parser::startParsing(Config &config, char *filename) {
    std::string holder;
    if (!filename) {
        filename = (char *)"config.conf";
    }
    struct stat fileInfo;
    if (stat(filename, &fileInfo) != 0) {
        std::cerr << strerror(errno) << std::endl;
        return (1);
    }
    else if (!S_ISREG(fileInfo.st_mode)) {
		std::cerr << filename << " is a Directory!" << std::endl;
        return (1);
    }
    std::ifstream f_read(filename);
	if (!f_read) {
		std::cerr << strerror(errno) << std::endl;
		return (1);
	}
    if (std::getline(f_read, holder, '\0')) {
        if (handleLines(config, holder)) {
            f_read.close();
            std::cerr << "Invalid server configuration!\n";
            return (1);
        }
    }
    else {
        f_read.close();
        std::cerr << "Invalid server configuration!\n";
        return (1);
    }
    f_read.close();
    return (0);
}

void Parser::joinStrings(std::vector<std::string>& holder) {
    for (std::vector<std::string>::iterator it = holder.begin(); it != holder.end(); ) {
        if (*it == ";" && it != holder.begin() && (it + 1) != holder.end()) {
            *(it - 1) += ";";
            std::vector<std::string>::iterator tmp_it = it;
            it++;
            holder.erase(tmp_it);
        }
        else
            it++;
    }
}

int Parser::handleLines(Config& config, std::string& line) {
    std::vector<std::string> holder;
    std::string tmp;
    replace(line, "\t", " ");
    replace(line, "\n", " ");
    replace(line, "}", " } ");
    replace(line, "{", " { ");
    replace(line, ";", "; ");
    std::stringstream strs(line);
    while(std::getline(strs, tmp, ' ')) {
        if (tmp != "")
            holder.push_back(tmp);
    }
    if (holder.empty())
        return (1);
    joinStrings(holder);
    for (size_t i = 0; i < holder.size(); i++) {
        if (holder[i] != "server")
            return (1);
        else {
            if (fillServer(config, holder, i))
                return (1);
        }
    }
    return (0);
}

void Parser::replace(std::string& line, std::string old_char, std::string new_char) {
    size_t index = line.find(old_char);
    while(index != std::string::npos) {
        line.erase(index, old_char.size());
        line.insert(index, new_char);
        index = line.find(old_char, index + new_char.size());
    }
}

int Parser::parseLocation(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    Server dummy_server;
    int retValue, listenCount, rootCount, indexCount, aIndexCount, bSizeCount, cgiDirCount, cPathCount, cExtCount;
    retValue = listenCount = rootCount = indexCount = aIndexCount = bSizeCount = cgiDirCount =  cPathCount = cExtCount = 0;
    while(index < holder.size()) {
        if (holder[index] == "listen" && !(listenCount++))
            retValue = setPortVar(holder, dummy_server, index);
        else if (holder[index] == "root" && !(rootCount++))
            retValue = setRootVar(holder, dummy_server, index);
        else if (holder[index] == "cgi_dir" && !(cgiDirCount++))
            retValue = setCgiDirVar(holder, tmp_server, index);
        else if (holder[index] == "index" && !(indexCount++))
            retValue = setIndexVar(holder, dummy_server, index);
        else if (holder[index] == "return")
            retValue = setRedirectVar(holder, dummy_server, index);
        else if (holder[index] == "autoindex" && !(aIndexCount++))
            retValue = setAutoindexVar(holder, dummy_server, index);
        else if (holder[index] == "server_name")
            retValue = setSnameVar(holder, dummy_server, index);
        else if (holder[index] == "error_page")
            retValue = setErrVar(holder, dummy_server, index);
        else if (holder[index] == "client_max_body_size" && !(bSizeCount++))
            retValue = setMaxBodyVar(holder, dummy_server, index);
        else if (holder[index] == "cgi_path" && !(cPathCount++))
            retValue = setCgiPathVar(holder, tmp_server, index);
        else if (holder[index] == "cgi_ext" && !(cExtCount++))
            retValue = setCgiExtVar(holder, tmp_server, index);
        else if (holder[index] == "location")
            retValue = handleLocation(holder, tmp_server, index);
        else if (holder[index] == "}")
            return (0);
        else
            return (1);
        if (retValue)
            return (1);
    }
    return (1);
}

int Parser::fillServer(Config& config, std::vector<std::string>& holder, size_t& index) {
    Server tmp_server;
    index++;
    if (index < holder.size() && holder[index] != "{")
        return (1);
    index++;
    size_t tmp_index = index;
    std::vector<std::string> old_holder = holder;
    bool sNameExist = false;
    int retValue, listenCount, rootCount, indexCount, aIndexCount, bSizeCount, cgiDirCount, cPathCount, cExtCount;
    retValue = listenCount = rootCount = indexCount = aIndexCount = bSizeCount = cgiDirCount =  cPathCount = cExtCount = 0;
    while(index < holder.size()) {
        if (holder[index] == "listen" && !(listenCount++))
            retValue = setPortVar(holder, tmp_server, index);
        else if (holder[index] == "root" && !(rootCount++))
            retValue = setRootVar(holder, tmp_server, index);
        else if (holder[index] == "cgi_dir" && !(cgiDirCount++))
            retValue = setCgiDirVar(holder, tmp_server, index);
        else if (holder[index] == "index" && !(indexCount++))
            retValue = setIndexVar(holder, tmp_server, index);
        else if (holder[index] == "return")
            retValue = setRedirectVar(holder, tmp_server, index);
        else if (holder[index] == "autoindex" && !(aIndexCount++))
            retValue = setAutoindexVar(holder, tmp_server, index);
        else if (holder[index] == "cgi_path" && !(cPathCount++))
            retValue = setCgiPathVar(holder, tmp_server, index);
        else if (holder[index] == "cgi_ext" && !(cExtCount++))
            retValue = setCgiExtVar(holder, tmp_server, index);
        else if (holder[index] == "server_name") {
            if (sNameExist == false) {
                tmp_server.getServerName().clear();
                sNameExist = true;
            }
            retValue = setSnameVar(holder, tmp_server, index);
        }
        else if (holder[index] == "error_page")
            retValue = setErrVar(holder, tmp_server, index);
        else if (holder[index] == "client_max_body_size" && !(bSizeCount++))
            retValue = setMaxBodyVar(holder, tmp_server, index);
        else if (holder[index] == "location")
            retValue = skipLocation(holder, index);
        else if (holder[index] == "}" && tmp_server.getCgiExt().size() == tmp_server.getCgiPath().size()) {
            if (parseLocation(old_holder, tmp_server, tmp_index) 
                || (tmp_server.getCgiDir().empty() && !tmp_server.getCgiExt().empty()))
                return (1);
            config.addServer(tmp_server);
            return (0);
        }
        else
            return (1);
        if (retValue)
            return (1);
    }
    return (1);
}

int Parser::isNumber(std::string& str) {
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        if (!std::isdigit(*i))
            return (0);
    }
    return (1);
}

int Parser::isValidValue(std::string& str) {
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        if (*i == ';')
            return (0);
    }
    return (1);
}

int Parser::setPortVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (setHostVar(holder, tmp_server, index))
        return (1);
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (!isNumber(holder[index]) || holder[index].empty() || !isValidValue(holder[index]) || holder[index].size() > 5) {
            return (1);
        }
        tmp_server.setPort(std::atoi(holder[index].c_str()));
        if (tmp_server.getPort() > 65535 || tmp_server.getPort() == 0)
            return (1);
        index++;
    }
    else
        return (1);
    return (0);
}

long long Parser::strToDecimal(std::string str) {
    std::stringstream s(str);
    long long holder;
    
    s >> holder;
    return (holder);
}

int Parser::setMaxBodyVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        int unit = 1;
        if ((*holder[index].rbegin() == 'k' || *holder[index].rbegin() == 'K'))
            unit = 1024;
        else if ((*holder[index].rbegin() == 'm' || *holder[index].rbegin() == 'M'))
            unit = 1024 * 1024;
        else if ((*holder[index].rbegin() == 'g' || *holder[index].rbegin() == 'G'))
            unit = 1024 * 1024 * 1024;
        if (unit != 1)
            holder[index].erase(holder[index].end() - 1);
        long long num = strToDecimal(holder[index]);
        if (!isNumber(holder[index]) || holder[index].size() > 19
            || (num == std::numeric_limits<long long>::max() && *holder[index].rbegin() != '7')
            || holder[index].empty() || !isValidValue(holder[index])
            || num > std::numeric_limits<long long>::max() / unit)
            return (1);
        tmp_server.setClientMaxBodySize(num * unit);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setMaxBodyVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        int unit = 1;
        if ((*holder[index].rbegin() == 'k' || *holder[index].rbegin() == 'K'))
            unit = 1024;
        else if ((*holder[index].rbegin() == 'm' || *holder[index].rbegin() == 'M'))
            unit = 1024 * 1024;
        else if ((*holder[index].rbegin() == 'g' || *holder[index].rbegin() == 'G'))
            unit = 1024 * 1024 * 1024;
        if (unit != 1)
            holder[index].erase(holder[index].end() - 1);
        long long num = strToDecimal(holder[index]);
        if (!isNumber(holder[index]) || holder[index].size() > 19
            || (num == std::numeric_limits<long long>::max() && *holder[index].rbegin() != '7')
            || holder[index].empty() || !isValidValue(holder[index])
            || num > std::numeric_limits<long long>::max() / unit)
            return (1);
        tmp_location.setClientMaxBodySize(num * unit);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRootVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_server.setRoot(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setCgiDirVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]) || holder[index] == "/" || *holder[index].begin() != '/')
            return (1);
        tmp_server.setCgiDir(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRootVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_location.setRoot(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setUploadVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_location.setUploadPath(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRedirectVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    int code = 0;
    if (index < holder.size() && *holder[index].rbegin() != ';') {
        code = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3)
            return (1);
        index++;
    }
    else
        return (1);
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_server.setRedirect(code, holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRedirectVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    int code = 0;
    if (index < holder.size() && *holder[index].rbegin() != ';') {
        code = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3)
            return (1);
        index++;
    }
    else
        return (1);
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_location.setRedirect(code, holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setIndexVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_server.setIndex(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setIndexVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_location.setIndex(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setAutoindexVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]) || (holder[index] != "on" && holder[index] != "off"))
            return (1);
        if (holder[index] == "on")
            tmp_location.setAutoindex(true);
        else
            tmp_location.setAutoindex(false);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setAutoindexVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]) || (holder[index] != "on" && holder[index] != "off"))
            return (1);
        if (holder[index] == "on")
            tmp_server.setAutoindex(true);
        else
            tmp_server.setAutoindex(false);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setHostVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    if (index < holder.size() && *holder[index].rbegin() == ';'
        && holder[index].find(':') != std::string::npos) {
        size_t pos = holder[index].find(':');
        std::string tmp = holder[index].substr(0, pos);
        holder[index].erase(0, pos + 1);
        if (tmp.empty())
            return (1);
        tmp_server.setHost(tmp);
    }
    return (0);
}

int Parser::setSnameVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    while (index < holder.size() && *holder[index].rbegin() != ';') {
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_server.setServerName(holder[index]);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        tmp_server.setServerName(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setErrVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    std::map<std::vector<int>, std::string> err;
    index++;
    size_t tmp_index = index;
    std::string path;
    while(tmp_index < holder.size() && *holder[tmp_index].rbegin() != ';')
        tmp_index++;
    if (tmp_index < holder.size() && *holder[tmp_index].rbegin() == ';') {
        std::string tmp_file = holder[tmp_index];
        tmp_file.erase(tmp_file.end() - 1);
        if (tmp_file.empty() || tmp_index == index)
            return (1);
        path = tmp_file;
    }
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        int tmp = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3 || tmp < 300 || tmp > 599)
            return (1);
        tmp_server.addErrorPage(tmp, path);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setErrVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    std::map<std::vector<int>, std::string> err;
    index++;
    size_t tmp_index = index;
    std::string path;
    while(tmp_index < holder.size() && *holder[tmp_index].rbegin() != ';')
        tmp_index++;
    if (tmp_index < holder.size() && *holder[tmp_index].rbegin() == ';') {
        std::string tmp_file = holder[tmp_index];
        tmp_file.erase(tmp_file.end() - 1);
        if (tmp_file.empty() || tmp_index == index)
            return (1);
        path = tmp_file;
    }
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        int tmp = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3 || tmp < 300 || tmp > 599)
            return (1);
        tmp_location.addErrorPage(tmp, path);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::checkDupMethod(std::string& holder, std::vector<std::string> tmp_holder) {
    for (std::vector<std::string>::iterator it = tmp_holder.begin(); it != tmp_holder.end(); it++) {
        if (holder == *it)
            return (0);
    }
    return (1);
}

int Parser::setAllowedMethodsVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    std::vector<std::string> tmp_holder;
    index++;
    tmp_location.setMethodsFlag(true);
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        if ((holder[index] == "POST" || holder[index] == "GET" || holder[index] == "DELETE") && checkDupMethod(holder[index], tmp_holder))
            tmp_holder.push_back(holder[index]);
        else
            return (1);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        if (holder[index] == ";" && !tmp_holder.empty()) {
            tmp_location.setAllowedMethods(tmp_holder);
            index++;
            return (0);
        }
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || !isValidValue(holder[index]))
            return (1);
        if ((holder[index] == "POST" || holder[index] == "GET" || holder[index] == "DELETE") && checkDupMethod(holder[index], tmp_holder))
            tmp_holder.push_back(holder[index]);
        else
            return (1);
        tmp_location.setAllowedMethods(tmp_holder);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setCgiPathVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        tmp_server.addCgiPath(holder[index]);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        tmp_server.addCgiPath(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setCgiExtVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        tmp_server.addCgiExt(holder[index]);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        tmp_server.addCgiExt(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::skipLocation(std::vector<std::string>& holder, size_t& index) {
    Location tmp_location;
    index++;
    int retValue, aMethodsCount, rootCount, indexCount, aIndexCount, cPathCount, cExtCount, bSizeCount;
    retValue = aMethodsCount = rootCount = indexCount = aIndexCount = cPathCount = cExtCount = bSizeCount =0;
    if (index < holder.size() && holder[index] != "{") {
        tmp_location.setURI(holder[index]);
        index++;
        if (index < holder.size() && holder[index] == "{") {
            index++;
            while(index < holder.size()) {
                if (holder[index] == "root" && !(rootCount++))
                    retValue = setRootVar(holder, tmp_location, index);
                else if (holder[index] == "index" && !(indexCount++))
                    retValue = setIndexVar(holder, tmp_location, index);
                else if (holder[index] == "upload_path")
                    retValue = setUploadVar(holder, tmp_location, index);
                else if (holder[index] == "autoindex" && !(aIndexCount++))
                    retValue = setAutoindexVar(holder, tmp_location, index);
                else if (holder[index] == "return")
                    retValue = setRedirectVar(holder, tmp_location, index);
                else if (holder[index] == "error_page")
                    retValue = setErrVar(holder, tmp_location, index);
                else if (holder[index] == "client_max_body_size" && !(bSizeCount++))
                    retValue = setMaxBodyVar(holder, tmp_location, index);
                else if (holder[index] == "allowed_methods" && !(aMethodsCount++))
                    retValue = setAllowedMethodsVar(holder, tmp_location, index);
                else if (holder[index] == "}") {
                    index++;
                    return (0);
                }
                else 
                    return (1);
                if (retValue)
                    return (1);
            }
        }
        else
            return (1);
    }
    else
        return (1);
    return (0);
}

int Parser::checkDupUri(std::string holder, Server& server) {
    for (std::vector<Location>::iterator it = server.getLocations().begin(); it != server.getLocations().end(); it++) {
        if (holder == it->getURI())
            return (1);
    }
    if (!server.getCgiDir().empty() && holder.find(server.getCgiDir()) != std::string::npos)
        return (1);
    return (0);
}

int Parser::handleLocation(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    Location tmp_location(tmp_server);
    index++;
    int retValue = 0;
    bool errExist = false;
    if (index < holder.size() && holder[index] != "{") {
        if (checkDupUri(holder[index], tmp_server))
            return (1);
        tmp_location.setURI(holder[index]);
        index++;
        if (index < holder.size() && holder[index] == "{") {
            index++;
            while(index < holder.size()) {
                if (holder[index] == "root")
                    retValue = setRootVar(holder, tmp_location, index);
                else if (holder[index] == "index")
                    retValue = setIndexVar(holder, tmp_location, index);
                else if (holder[index] == "upload_path")
                    retValue = setUploadVar(holder, tmp_location, index);
                else if (holder[index] == "autoindex")
                    retValue = setAutoindexVar(holder, tmp_location, index);
                else if (holder[index] == "return")
                    retValue = setRedirectVar(holder, tmp_location, index);
                else if (holder[index] == "error_page") {
                    if (errExist == false) {
                        tmp_location.getErrorPage().clear();
                        errExist = true;
                    }
                    retValue = setErrVar(holder, tmp_location, index);
                }
                else if (holder[index] == "allowed_methods")
                    retValue = setAllowedMethodsVar(holder, tmp_location, index);
                else if (holder[index] == "client_max_body_size")
                    retValue = setMaxBodyVar(holder, tmp_location, index);
                else if (holder[index] == "}") {
                    if (*tmp_location.getURI().begin() == '/')
                        tmp_server.addLocation(tmp_location);
                    index++;
                    return (0);
                }
                else 
                    return (1);
                if (retValue)
                    return (1);
            }
        }
        else
            return (1);
    }
    else
        return (1);
    return (0);
}
