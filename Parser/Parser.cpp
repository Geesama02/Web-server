/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 14:31:56 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/23 16:54:31 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(Config &config, char *filename) {
    (void)config;
    std::string holder;
    if (!filename)
        return;
    std::ifstream f_read(filename);
	if (!f_read)
	{
		std::cerr << "Failed to open file!\n";
		return;
	}
    while(std::getline(f_read, holder))
    {
        // std::cout << holder << std::endl;
        handle_line(config, holder);
    }
}

int Parser::handle_line(Config& config, std::string& line) {
    (void)config;
    replace(line);
    std::cout << line << std::endl;
    return (0);
}
void Parser::replace(std::string& line) {
    size_t index = line.find(' ');
    while(index != std::string::npos)
    {
        line.replace(index, 1, 1, '\t');
        index = line.find(' ', index);
    }
}