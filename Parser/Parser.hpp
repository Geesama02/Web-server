/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:46:14 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/08 13:37:46 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include "../Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <signal.h>

class Parser {
    public:
        int startParsing(Config &config, char *filename);
        int handleLines(Config& config, std::string& line);
        static void replace(std::string& line, std::string old_char, std::string new_char);
        int fillServer(Config& config, std::vector<std::string>& holder, size_t& index);
        int parseLocation(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        static int isNumber(std::string& str);
        int isValidValue(std::string& str);
        long long strToDecimal(std::string str);
        int setPortVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setRootVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setRootVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setCgiDirVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setUploadVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setHostVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setIndexVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setIndexVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setRedirectVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setRedirectVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setAutoindexVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setAutoindexVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setSnameVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setErrVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setErrVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setMaxBodyVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setMaxBodyVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setAllowedMethodsVar(std::vector<std::string>& holder, Location& tmp_location, size_t& index);
        int setCgiPathVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int setCgiExtVar(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int handleLocation(std::vector<std::string>& holder, Server& tmp_server, size_t& index);
        int skipLocation(std::vector<std::string>& holder, size_t& index);
        int checkDupUri(std::string holder, Server& server);
        void joinStrings(std::vector<std::string>& holder);
        int checkDupMethod(std::string& holder, std::vector<std::string> tmp_holder);
};

#endif