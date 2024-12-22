/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 16:52:32 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/22 11:17:15 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>

class Location {
    private:
        std::string URI;
        std::string root;
        std::vector<std::string> error_page;
        std::vector<std::string> allowed_methods;
        std::string index;
        bool autoindex;
        std::string redirect;
    public:
        // Constructor
        Location();

        // Getters
        std::string getURI();
        std::string getRoot();
        std::vector<std::string> getErrorPage();
        std::string getIndex();
        bool getAutoindex();
        std::vector<std::string> getAllowedMethods();
        std::string getRedirect();
        
        // Setters
        void setURI(std::string& name);
        void setRoot(std::string& name);
        void setErrorPage(std::vector<std::string>& n_ep);
        void setIndex(std::string& str);
        void setAutoindex(bool n_autoindex);
        void setAllowedMethods(std::vector<std::string>& methods);
        void setRedirect(std::string& page);
};