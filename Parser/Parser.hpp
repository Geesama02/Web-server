/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:46:14 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/24 16:48:18 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include "../Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

class Parser {
    public:
        Parser(Config &config, char *filename);
        int handle_line(Config& config, std::string& line);
        void replace(std::string& line, char old_char, char new_char);
        int fill_server(Config& config, std::vector<std::string>& holder, size_t& index);
        int isNumber(std::string& str);
        int set_port_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int set_root_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int set_host_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int set_sname_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int set_err_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int set_max_body_var(std::vector<std::string>& holder, Server& tmp_server, size_t& index);

};

#endif