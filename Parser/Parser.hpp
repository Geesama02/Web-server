/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 10:46:14 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/23 16:52:55 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include "../Config/Config.hpp"
#include <fstream>

class Parser {
    public:
        Parser(Config &config, char *filename);
        int handle_line(Config& config, std::string& line);
        void replace(std::string& line);

};

#endif