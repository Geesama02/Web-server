/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 14:31:56 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/27 11:29:13 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

int Parser::start_parsing(Config &config, char *filename) {
    std::string holder;
    if (!filename)
        return (1);
    std::ifstream f_read(filename);
	if (!f_read) {
		std::cerr << "Failed to open file!\n";
		return (1);
	}
    if (std::getline(f_read, holder, '\0')) {
        if (handle_line(config, holder)) {
            std::cerr << "Invalid server configuration!\n";
            return (1);
        }
    }
    return (0);
}

int Parser::handle_line(Config& config, std::string& line) {
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
            if (fill_server(config, holder, i))
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

int Parser::fill_server(Config& config, std::vector<std::string>& holder, size_t& index) {
    (void)config;
    Server tmp_server;
    index++;
    if (index < holder.size() && holder[index] != "{")
        return (1);
    index++;
    while(index < holder.size()) {
        if (holder[index] == "listen") {
            if (set_port_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "host") {
            if (set_host_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "root") {
            if (set_root_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "index") {
            if (set_index_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "return") {
            if (set_redirect_var(holder, tmp_server, index))
                return (1);
            // std::cout << "Redirect -> " << tmp_server.getRedirect() << std::endl;
        }
        else if (holder[index] == "server_name") {
            if (set_sname_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "error_page") {
            if (set_err_var(holder, tmp_server, index))
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
            if (set_max_body_var(holder, tmp_server, index))
                return (1);
        }
        else if (holder[index] == "location") {
            if (handle_location(holder, tmp_server, index))
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

int Parser::set_port_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    // std::cout << "*holder[index].rbegin()" << std::endl;
    index++;
    // std::cout << str << std::endl;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (!isNumber(holder[index]) || holder[index].empty() || holder[index].size() > 5) {
            std::cout << "Invalid Port Number!\n";
            return (1);
        }
        tmp_server.setPort(std::atoi(holder[index].c_str()));
        if (tmp_server.getPort() > 65535 || tmp_server.getPort() == 0)
            return (1);
        // std::cout << "Port: " << tmp_server.getPort() << std::endl;
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::set_max_body_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    index++;
    if (index < holder.size() && *holder[index].rbegin() == ';') {
        holder[index].erase(holder[index].end() - 1);
        if (!isNumber(holder[index]) || holder[index].size() > 10
            || atol(holder[index].c_str()) > 2147483647 || holder[index].empty())
            return (1);
        tmp_server.setClientMaxBodySize(std::atoi(holder[index].c_str()));
        index++;
    }
    else
        return (1);
    return (0);
}

int Parser::set_root_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_root_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_redirect_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_redirect_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_index_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_index_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_autoindex_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_host_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_sname_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_err_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
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

int Parser::set_err_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_allowed_methods_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_cgi_path_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::set_cgi_ext_var(std::vector<std::string>& holder, Location& tmp_location, size_t& index) {
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

int Parser::handle_location(std::vector<std::string>& holder, Server& tmp_server, size_t& index) {
    Location tmp_location;
    index++;
    if (index < holder.size() && holder[index] != "{") {
        tmp_location.setURI(holder[index]);
        index++;
        if (index < holder.size() && holder[index] == "{") {
            index++;
            while(index < holder.size()) {
                if (holder[index] == "root") {
                    if (set_root_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "index") {
                    if (set_index_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "autoindex") {
                    if (set_autoindex_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "return") {
                    if (set_redirect_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "error_page") {
                    if (set_err_var(holder, tmp_location, index))
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
                    if (set_allowed_methods_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "cgi_path") {
                    if (set_cgi_path_var(holder, tmp_location, index))
                        return (1);
                }
                else if (holder[index] == "cgi_ext") {
                    if (set_cgi_ext_var(holder, tmp_location, index))
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