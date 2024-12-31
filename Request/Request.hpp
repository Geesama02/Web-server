/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:15:32 by oait-laa          #+#    #+#             */
/*   Updated: 2024/12/31 17:03:59 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

class Request {
    private:
        std::map<std::string, std::string> Headers;
        std::string method;
        std::string path;
        std::string version;
        // std::string host;
        // std::string connection;
        // std::string content_length;
        // std::string content_type;
        // std::string accept;
        std::string body;
    public:
        // Getters
        std::map<std::string, std::string> getHeaders();
        std::string getMethod();
        std::string getPath();
        std::string getVersion();
        // std::string getHost();
        // std::string getConnection();
        // std::string getContentLength();
        // std::string getContentType();
        // std::string getAccept();
        std::string getBody();

        // Setters
        // void setMethod(std::map<std::string, std::string>);
        void setMethod(const std::string& m);
        void setPath(const std::string& p);
        void setVersion(const std::string& v);
        // void setHost(const std::string& h);
        // void setConnection(const std::string& c);
        // void setContentLength(const std::string& cl);
        // void setContentType(const std::string& ct);
        // void setAccept(const std::string& a);
        void setBody(const std::string& b);

        // Functions
        int parse(std::string buffer);
        std::vector<std::string> split(std::string buffer, int full, char del);
        void to_lower(std::string& str);
    
};

#endif