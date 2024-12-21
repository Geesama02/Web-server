/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 16:52:32 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/21 16:59:36 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>

class Location {
    private:
        std::string root;
        std::vector<std::string> error_page;
        std::vector<std::string> allowed_methods;
        std::string index;
        bool autoindex;
        std::string redirect;
};