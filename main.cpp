#include "Parser/Parser.hpp"
#include "Request/Request.hpp"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << "Invalid Number of parameters\n";
        return (1);
    }
    Config config;
    Parser parser;
    if (parser.startParsing(config, argv[1]))
        return (1);
    if (config.startServers())
        return (1);
    return (0);
}
