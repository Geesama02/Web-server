// #include <iostream>
#include "Parser/Parser.hpp"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << "Invalid Number of parameters\n";
        return (1);
    }
    Config config;
    Parser parser;
    if (parser.start_parsing(config, argv[1]))
        return (1);
    if (config.init_sockets())
        return (1);
    return (0);
}