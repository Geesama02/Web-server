/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 14:31:56 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/18 15:25:07 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

int Parser::startParsing(Config &config, char *filename) {
    std::string holder;
    if (!filename) {
        Server defaultServer;
        config.addServer(defaultServer);
        return (0);
    }
    std::ifstream f_read(filename);
	if (!f_read) {
		std::cerr << "Failed to open file!\n";
		return (1);
	}
    if (std::getline(f_read, holder, '\0')) {
        if (handleLines(config, holder)) {
            std::cerr << "Invalid server configuration!\n";
            return (1);
        }
    }
    return (0);
}

int Parser::handleLines(Config& config, std::string& line) {
    std::vector<std::string> holder;
    std::string tmp;
    replace(line, '\t', ' ');
    replace(line, '\n', ' ');
    std::stringstream strs(line);
    while(std::getline(strs, tmp, ' ')) {
        if (tmp != "")
            holder.push_back(tmp);
    }
    for (size_t i = 0; i < holder.size(); i++) {
        if (holder[i] != "server") {
            return (1);
        }
        else {
            if (fillServer(config, holder, i))
                return (1);
        }
    }
    return (0);
}

void Parser::replace(std::string& line, char old_char, char new_char) {
    size_t index = line.find(old_char);
    while(index != std::string::npos) {
        line.replace(index, 1, 1, new_char);
        index = line.find(old_char, index);
    }
}

int Parser::fillServer(Config& config, std::vector<std::string>& holder, size_t& index) {
    (void)config;
    Server tmp_server;
    index++;
    if (index < holder.size() && holder[index] != "{")
        return (1);
    index++;
    while(index < holder.size()) {
        if (holder[index] == "listen") {
            if (setPortVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "host") {
            if (setHostVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "root") {
            if (setRootVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "index") {
            if (setIndexVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "return") {
            if (setRedirectVar(holder, tmp_server, index))
                return (1);
            // std::cout << "Redirect -> " << tmp_server.getRedirect() << std::endl;
        }
        else if (holder[index] == "server_name") {
            if (setSnameVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "error_page") {
            if (setErrVar(holder, tmp_server, index))
                return (1);
            // for (std::map<std::vector<int>, std::string>::iterator it = tmp_server.getErrorPage().begin(); it != tmp_server.getErrorPage().end(); it++)
            // {
            //     std::cout << "{ ";
            //     for (std::vector<int>::const_iterator it2 = it->first.begin(); it2 != it->first.end(); it2++)
            //     {
            //         std::cout << *it2 << ' ';
            //     }
            //     std::cout << " }, " << it->second << std::endl;
            // }
            
        }
        else if (holder[index] == "client_max_body_size") {
            if (setMaxBodyVar(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "location") {
            if (handleLocation(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "}") {
            config.addServer(tmp_server);
            return (0);
        }
        else
            return (1);
    }
    return (0);
}

int Parser::isNumber(std::string& str) {
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        if (!std::isdigit(*i))
            return (0);
    }
    return (1);
}

int Parser::setPortVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (!isNumber(holder[index]) || holder[index].empty() || holder[index].size() > 5) {
            std::cout << "Invalid Port Number!\n";
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
        if (!isNumber(holder[index]) || holder[index].size() > 12
            || strToDecimal(holder[index]) > 107374182400 || holder[index].empty())
            return (1);
        tmp_server.setClientMaxBodySize(strToDecimal(holder[index]));
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRootVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    // std::cout << "inside" << std::endl;
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_server.setRoot(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRootVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    // std::cout << "inside" << std::endl;
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_location.setRoot(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRedirectVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_server.setRedirect(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setRedirectVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_location.setRedirect(holder[index]);
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
        if (holder[index].empty())
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
        if (holder[index].empty())
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
        if (holder[index].empty() || (holder[index] != "on" && holder[index] != "off"))
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

int Parser::setHostVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    // std::cout << "inside" << std::endl;
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_server.setHost(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setSnameVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    // std::cout << "inside" << std::endl;
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty())
            return (1);
        tmp_server.setServerName(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setErrVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    std::vector<int> nums;
    std::map<std::vector<int>, std::string> err;
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        int tmp = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3 || tmp < 100 || tmp > 599)
            return (1);
        nums.push_back(atoi(holder[index].c_str()));
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || nums.empty())
            return (1);
        err.insert(std::pair<std::vector<int>, std::string>(nums, holder[index]));
        tmp_server.setErrorPage(err);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setErrVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    std::vector<int> nums;
    std::map<std::vector<int>, std::string> err;
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        int tmp = atoi(holder[index].c_str());
        if (!isNumber(holder[index]) || holder[index].size() > 3 || tmp < 100 || tmp > 599)
            return (1);
        nums.push_back(atoi(holder[index].c_str()));
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || nums.empty())
            return (1);
        err.insert(std::pair<std::vector<int>, std::string>(nums, holder[index]));
        tmp_location.setErrorPage(err);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setAllowedMethodsVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    std::vector<std::string> tmp_holder;
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        if (holder[index] == "POST" || holder[index] == "GET" || holder[index] == "DELETE")
            tmp_holder.push_back(holder[index]);
        else
            return (1);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        if (holder[index].empty() || tmp_holder.empty())
            return (1);
        if (holder[index] == "POST" || holder[index] == "GET" || holder[index] == "DELETE")
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

int Parser::setCgiPathVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        tmp_location.addCgiPath(holder[index]);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        tmp_location.addCgiPath(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::setCgiExtVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
    index++;
    while(index < holder.size() && *holder[index].rbegin() != ';') {
        tmp_location.addCgiExt(holder[index]);
        index++;
    }
    if (index < holder.size() && *holder[index].rbegin() == ';')
    {
        holder[index].erase(holder[index].end() - 1);
        tmp_location.addCgiExt(holder[index]);
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::handleLocation(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    Location tmp_location;
    index++;
    if (index < holder.size() && holder[index] != "{") {
        tmp_location.setURI(holder[index]);
        index++;
        if (index < holder.size() && holder[index] == "{") {
            index++;
            while(index < holder.size()) {
                if (holder[index] == "root") {
                    if (setRootVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "index") {
                    if (setIndexVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "autoindex") {
                    if (setAutoindexVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "return") {
                    if (setRedirectVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "error_page") {
                    if (setErrVar(holder, tmp_location, index))
                        return (1);
                    // std::cout << "size: " << tmp_location.getErrorPage().size() << std::endl;
                    // for (std::map<std::vector<int>, std::string>::iterator it = tmp_location.getErrorPage().begin(); it != tmp_location.getErrorPage().end(); it++)
                    // {
                    //     std::cout << "{ ";
                    //     // std::cout << "Inside" << std::endl;
                    //     for (std::vector<int>::const_iterator it2 = it->first.begin(); it2 != it->first.end(); it2++)
                    //         std::cout << *it2 << ' ';
                    //     std::cout << "}, " << it->second << std::endl;
                    // }
                    
                }
                else if (holder[index] == "allowed_methods") {
                    if (setAllowedMethodsVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "cgi_path") {
                    if (setCgiPathVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "cgi_ext") {
                    if (setCgiExtVar(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "}") {
                    tmp_server.addLocation(tmp_location);
                    index++;
                    return (0);
                }
                else 
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