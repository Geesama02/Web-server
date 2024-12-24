/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 14:31:56 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/24 16:52:32 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(Config &config, char *filename) {
    (void)config;
    std::string holder;
    if (!filename)
        return;
    std::ifstream f_read(filename);
	if (!f_read) {
		std::cerr << "Failed to open file!\n";
		return;
	}
    if (std::getline(f_read, holder, '\0')) {
        if (handle_line(config, holder)) {
            std::cerr << "Invalid server configuration!\n";
            return;
        }
    }
}

int Parser::handle_line(Config& config, std::string& line) {
    (void)config;
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
        if (holder[0] != "server")
            return (1);
        else {
            if (fill_server(config, holder, i))
                return (1);
        }
            
            
        // std::cout << "vector value -> " << holder[i] << std::endl;
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
            // std::cout << "Max -> " << tmp_server.getClientMaxBodySize() << std::endl;
        }
        else
            index++;
    }
    return (0);
}

int Parser::isNumber(std::string& str) {
    for(std::string::iterator i = str.begin(); i != str.end(); i++) {
        // std::cout <<"|"<< *i <<"|" << std::endl;
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
        if (!isNumber(holder[index]) || holder[index].empty()) {
            std::cout << "Invalid Port Number!\n";
            return (1);
        }
        tmp_server.setPort(std::atoi(holder[index].c_str()));
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
        if (!isNumber(holder[index]) || holder[index].empty())
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
        // std::cout <<"|"<< holder[index] <<"|" << std::endl;
        if (!isNumber(holder[index]))
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